import torch
import torch.nn as nn
import torch.optim as optim
from collections import deque
from tqdm import tqdm
# Assuming these come from your 'bindings' or external logic
import bot_tester 

# Constants
LAYER_SIZES = [97, 64, 16, 1]
LEARNING_RATE = 1e-3
LAMBDA = 0.85
GAMMA = 0.99
NUM_GAMES = 100000

# ===================== Neural Network =====================
class ValueNet(nn.Module):
    def __init__(self):
        super().__init__()
        self.model = nn.Sequential(
            nn.Linear(LAYER_SIZES[0], LAYER_SIZES[1]),
            nn.ReLU(),
            nn.Linear(LAYER_SIZES[1], LAYER_SIZES[2]),
            nn.ReLU(),
            nn.Linear(LAYER_SIZES[2], LAYER_SIZES[3]),
            nn.Tanh()  # Squashes output between -1 (Black wins) and 1 (White wins)
        )

    def forward(self, x):
        return self.model(x).squeeze(-1)

# ===================== TD(lambda) Trainer =====================
class TDLambdaTrainer:
    def __init__(self, model, lr=LEARNING_RATE, lambd=LAMBDA, gamma=GAMMA):
        self.model = model
        self.lr = lr
        self.lambd = lambd
        self.gamma = gamma
        # Eligibility traces for every parameter
        self.traces = [torch.zeros_like(p) for p in self.model.parameters()]

    def reset_traces(self):
        for t in self.traces:
            t.zero_()

    def update(self, state, next_state_value, reward=0, terminal=False):
        # 1. Forward pass for current state
        current_value = self.model(state)
        
        # 2. TD Error (delta)
        if terminal:
            delta = reward - current_value
        else:
            delta = (reward + self.gamma * next_state_value) - current_value

        # 3. Calculate gradients (dV/d_weights)
        self.model.zero_grad()
        current_value.backward()

        # 4. Update traces and weights
        with torch.no_grad():
            delta_val = delta.item()
            for i, p in enumerate(self.model.parameters()):
                # Update trace: e = (gamma * lambda) * e + grad
                self.traces[i] = (self.gamma * self.lambd) * self.traces[i] + p.grad
                # Update weights: theta = theta + alpha * delta * e
                p += self.lr * delta_val * self.traces[i]

        return current_value.item()
    
def load_model():
    # 1. Create the model instance first
    model = ValueNet()

    # 2. Load the saved data
    # Use weights_only=True for security if you're on a newer version of Torch
    checkpoint = torch.load("src/ai_models/stupid", weights_only=False)

    # 3. Load the weights into the model
    model.load_state_dict(checkpoint['model_state_dict'])

    # 4. Set to evaluation mode (disables dropout/batchnorm if you add them later)
    model.eval()
    return model
# ==================== Test / Play against Heuristic ==========


def play_games_against_heuristic(model, num_test_games=2):
    wins, losses, draws = 0, 0, 0
    
    for game_idx in tqdm(range(num_test_games)):
        turn_counter = 0
        gamestate = bot_tester.get_start_position()
        game_history = [gamestate]

        # Even game_idx: AI is White (0), Odd game_idx: AI is Black (1)
        ai_side = game_idx % 2 
        game_over = False

        while not game_over:
            # 0 for White, 1 for Black
            side_to_move = 0 if turn_counter % 2 != 0 else 1
            
            is_ai_turn = (side_to_move == ai_side)

            if not is_ai_turn:
                # 1. Heuristic Move
                gamestate = bot_tester.make_turn(gamestate, turn_counter, 8)
            else:
                # 2. AI Move (Neural Net)
                possible_gamestates = bot_tester.generate_next_gamestates(gamestate, turn_counter)
                
                if not possible_gamestates:
                    break 

                eval_list = []
                for gs in possible_gamestates:
                    neighbor_counts = [bot_tester.count_neighbors(gs, bee) for bee in gs]
                    moves = bot_tester.legal_moves(gs, turn_counter)
                    moves += [0] * (80 - len(moves))
                    
                    features = gs + neighbor_counts + moves + [side_to_move]
                    feat_tensor = torch.tensor(features, dtype=torch.float32)
                    
                    with torch.no_grad():
                        val = model(feat_tensor).item()
                    eval_list.append((val, gs))

                # If side_to_move is 1 (Black), multiplier is -1 (aiming for Black win)
                # If side_to_move is 0 (White), multiplier is 1 (aiming for White win)
                multiplier = 1 if side_to_move == 0 else -1
                eval_list.sort(key=lambda x: x[0] * multiplier, reverse=True)
                
                gamestate = eval_list[0][1]

            game_history.append(gamestate)
            res = bot_tester.game_terminated(gamestate, turn_counter, game_history)
            bot_tester.print_gamestate(gamestate)
            
            if res != 0:
                game_over = True
                if res == 3:
                    draws += 1
                elif (res == 1 and ai_side == 0) or (res == 2 and ai_side == 1):
                    wins += 1
                else:
                    losses += 1

            turn_counter += 1
        print(f"\n Turn counter: ", turn_counter)

    print(f"\n--- Results vs Heuristic (1=Black, 0=White) ---")
    print(f"Games Played: {num_test_games}")
    print(f"AI Wins: {wins} | AI Losses: {losses} | Draws: {draws}")
    if num_test_games > 0:
        print(f"Win Rate: {(wins / num_test_games) * 100:.1f}%")

def train_and_save_model():
    torch.manual_seed(0)
    model = ValueNet()
    trainer = TDLambdaTrainer(model)

    for game_idx in tqdm(range(NUM_GAMES)):
        trainer.reset_traces()
        gamestate = bot_tester.get_start_position()
        game_over = False
        turn_counter = 0
        game_history = []
        
        # track state to perform update on the NEXT step
        prev_state_tensor = None

        while not game_over:
            turn_counter += 1
            side_to_move = turn_counter % 2
            
            # 1. Generate moves and find the best one
            possible_gamestates = bot_tester.generate_next_gamestates(gamestate, turn_counter)
            
            evaluations = []
            for gs in possible_gamestates:
                # Build your feature vector (57 elements)
                neighbor_counts = []
                for bee in gs:
                    neighbor_counts.append(bot_tester.count_neighbors(gs, bee))
                legal_moves = bot_tester.legal_moves(gs, turn_counter)
                legal_moves += [0] * (80 - len(legal_moves)) # Padding to fix dim
                features = gs + neighbor_counts + legal_moves + [side_to_move]
                feat_tensor = torch.tensor(features, dtype=torch.float32)
                
                with torch.no_grad():
                    val = model(feat_tensor).item()
                evaluations.append((feat_tensor, val))

            # 2. Pick best move based on side to move
            # White: 1.0, Black -1.0
            multiplier = 1 if side_to_move == 1 else -1
            evaluations.sort(key=lambda x: x[1] * multiplier, reverse=True)
            
            best_state_tensor, best_value = evaluations[0]

            # 3. TD Update: Update the PREVIOUS state using the CURRENT state's value
            if prev_state_tensor is not None:
                trainer.update(prev_state_tensor, best_value, reward=0, terminal=False)

            prev_state_tensor = best_state_tensor
            gamestate = possible_gamestates[0] # Update game board
            game_history.append(gamestate)
            
            # 4. Check for terminal condition
            result = bot_tester.game_terminated(gamestate, turn_counter, game_history)
            if result != 0: 
                game_over = True
                if result == 2:
                    result = -1
                if result == 3:
                    result = 0
                # Final update for the last state reached
                trainer.update(prev_state_tensor, 0, reward=result, terminal=True)

    # ===================== Save & Summary =====================
    save_path = "src/ai_models/stupid"
    torch.save({
        'model_state_dict': model.state_dict(),
        'layer_sizes': LAYER_SIZES,
        'learning_rate': LEARNING_RATE,
    }, save_path)
    
    print(f"\nTraining complete. Model saved to {save_path}")
    
    # Quick Check: Does the model favor White or Black at start?
    initial_feat = torch.zeros(LAYER_SIZES[0])
    with torch.no_grad():
        initial_val = model(initial_feat).item()
    print(f"Neutral state evaluation: {initial_val:.4f}")

# ===================== Example Usage / Game Loop =====================
if __name__ == "__main__":
    train_and_save_model()
    play_games_against_heuristic(load_model())
