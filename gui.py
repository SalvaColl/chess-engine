import pygame
import chess
import subprocess
import sys

human_color = chess.WHITE
if len(sys.argv) > 1 and sys.argv[1].lower() == "black":
    human_color = chess.BLACK
    print("You are playing as Black.")
else:
    print("You are playing as White.")

ENGINE_PATH = "./engine.exe"
engine = subprocess.Popen(
    [ENGINE_PATH],
    universal_newlines=True,
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    bufsize=1
)

def send_command(cmd):
    engine.stdin.write(cmd + "\n")
    engine.stdin.flush()

def wait_for(expected_prefix):
    while True:
        line = engine.stdout.readline().strip()
        if line.startswith(expected_prefix):
            return line

send_command("uci")
wait_for("uciok")
send_command("isready")
wait_for("readyok")

pygame.init()
pygame.mixer.init()

try:
    move_sound = pygame.mixer.Sound("Move.ogg")
    capture_sound = pygame.mixer.Sound("Capture.ogg")
except FileNotFoundError:
    move_sound = None
    capture_sound = None

BOARD_SIZE = 800
SIDEBAR_WIDTH = 200
WIDTH, HEIGHT = BOARD_SIZE + SIDEBAR_WIDTH, BOARD_SIZE
SQ_SIZE = BOARD_SIZE // 8
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("CP Chess Engine")

pygame.font.init()
ui_font = pygame.font.SysFont("segoeui", 22, bold=True)
ui_font_small = pygame.font.SysFont("segoeui", 18)

current_eval = 0.0
current_eval_text = "0.00"
current_depth = "0 / 0"
current_nps = 0
current_pv = ""

font = pygame.font.SysFont("segoeuisymbol", int(SQ_SIZE * 0.8))

WHITE_SQUARE = (240, 217, 181)
BLACK_SQUARE = (181, 136, 99)
HIGHLIGHT = (186, 202, 68)
DOT_COLOR = (130, 151, 105)

PIECE_UNICODE = {
    'P': '♙', 'N': '♘', 'B': '♗', 'R': '♖', 'Q': '♕', 'K': '♔',
    'p': '♟', 'n': '♞', 'b': '♝', 'r': '♜', 'q': '♛', 'k': '♚'
}

def draw_board(board, selected_sq, valid_moves, last_move, right_clicked_squares, eval_score, eval_text_str, depth_str, nps_val, pv_str):
    for r in range(8):
        for c in range(8):
            color = WHITE_SQUARE if (r + c) % 2 == 0 else BLACK_SQUARE
            rect = pygame.Rect(c * SQ_SIZE, r * SQ_SIZE, SQ_SIZE, SQ_SIZE)
            
            if human_color == chess.WHITE:
                sq_index = (7 - r) * 8 + c
            else:
                sq_index = r * 8 + (7 - c)
                
            pygame.draw.rect(screen, color, rect)
            
            if sq_index in right_clicked_squares:
                highlight_surface = pygame.Surface((SQ_SIZE, SQ_SIZE))
                highlight_surface.set_alpha(150)
                highlight_surface.fill((235, 97, 80)) 
                screen.blit(highlight_surface, (c * SQ_SIZE, r * SQ_SIZE))

            if last_move and sq_index in (last_move.from_square, last_move.to_square):
                highlight_surface = pygame.Surface((SQ_SIZE, SQ_SIZE))
                highlight_surface.set_alpha(100) 
                highlight_surface.fill((255, 255, 0)) 
                screen.blit(highlight_surface, (c * SQ_SIZE, r * SQ_SIZE))
                
            if selected_sq == sq_index:
                highlight_surface = pygame.Surface((SQ_SIZE, SQ_SIZE))
                highlight_surface.set_alpha(100)
                highlight_surface.fill((0, 0, 255)) 
                screen.blit(highlight_surface, (c * SQ_SIZE, r * SQ_SIZE))
            
            if sq_index in valid_moves:
                center = (c * SQ_SIZE + SQ_SIZE // 2, r * SQ_SIZE + SQ_SIZE // 2)
                pygame.draw.circle(screen, DOT_COLOR, center, SQ_SIZE // 6)
            
            piece = board.piece_at(sq_index)
            if piece:
                text = font.render(PIECE_UNICODE[piece.symbol()], True, (0, 0, 0))
                text_rect = text.get_rect(center=rect.center)
                screen.blit(text, text_rect)

    pygame.draw.rect(screen, (40, 40, 40), (BOARD_SIZE, 0, SIDEBAR_WIDTH, HEIGHT))

    clamped_eval = max(-10.0, min(10.0, eval_score))
    white_percentage = 0.5 + (clamped_eval / 20.0)
    white_height = int(HEIGHT * white_percentage)
    black_height = HEIGHT - white_height
    
    pygame.draw.rect(screen, (20, 20, 20), (BOARD_SIZE, 0, 30, black_height)) 
    pygame.draw.rect(screen, (230, 230, 230), (BOARD_SIZE, black_height, 30, white_height)) 
    
    text_x = BOARD_SIZE + 45
    
    eval_surface = ui_font.render(f"Eval: {eval_text_str}", True, (255, 255, 255))
    screen.blit(eval_surface, (text_x, 20))
    
    depth_text = ui_font_small.render("Depth:", True, (150, 150, 150))
    depth_val = ui_font_small.render(f"{depth_str}", True, (200, 200, 255))
    screen.blit(depth_text, (text_x, 80))
    screen.blit(depth_val, (text_x, 100))
    
    nps_text = ui_font_small.render("Speed:", True, (150, 150, 150))
    nps_val_surf = ui_font_small.render(f"{nps_val:,} NPS", True, (255, 200, 150))
    screen.blit(nps_text, (text_x, 140))
    screen.blit(nps_val_surf, (text_x, 160))
    
    pv_label = ui_font_small.render("Best Line:", True, (150, 150, 150))
    screen.blit(pv_label, (text_x, 200))
    
    words = pv_str.split()
    pv_line_1 = " ".join(words[:2]) if len(words) > 0 else ""
    pv_line_2 = " ".join(words[2:4]) if len(words) > 2 else ""
    
    pv_surf_1 = ui_font_small.render(pv_line_1, True, (150, 255, 150))
    pv_surf_2 = ui_font_small.render(pv_line_2, True, (150, 255, 150))
    screen.blit(pv_surf_1, (text_x, 220))
    screen.blit(pv_surf_2, (text_x, 240))

board = chess.Board()
selected_sq = None
valid_moves = []
running = True
game_over_printed = False
last_move = None
right_clicked_squares = set()

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            
        elif event.type == pygame.MOUSEBUTTONDOWN:
            x, y = pygame.mouse.get_pos()
            col = x // SQ_SIZE
            row = y // SQ_SIZE
            
            if human_color == chess.WHITE:
                sq = (7 - row) * 8 + col
            else:
                sq = row * 8 + (7 - col)

            if event.button == 1: 
                right_clicked_squares.clear() 
                
                if board.turn == human_color:
                    if selected_sq is None:
                        piece = board.piece_at(sq)
                        if piece and piece.color == human_color:
                            selected_sq = sq
                            valid_moves = [m.to_square for m in board.legal_moves if m.from_square == sq]
                    else:
                        move = chess.Move(selected_sq, sq)
                        
                        if board.piece_at(selected_sq) and board.piece_at(selected_sq).piece_type == chess.PAWN:
                            if chess.square_rank(sq) in [0, 7]:
                                move = chess.Move(selected_sq, sq, promotion=chess.QUEEN)

                        if move in board.legal_moves:
                            is_capture = board.is_capture(move)
                            board.push(move)
                            last_move = None
                            
                            if is_capture and capture_sound:
                                capture_sound.play()
                            elif move_sound:
                                move_sound.play()
                        
                        selected_sq = None
                        valid_moves = []

            elif event.button == 3:
                if sq in right_clicked_squares:
                    right_clicked_squares.remove(sq) 
                else:
                    right_clicked_squares.add(sq)    

    draw_board(board, selected_sq, valid_moves, last_move, right_clicked_squares, current_eval, current_eval_text, current_depth, current_nps, current_pv)
    pygame.display.flip()

    is_drawn = board.can_claim_draw()
    
    if board.turn != human_color and not board.is_game_over() and not is_drawn:
        pygame.event.pump() 
        
        move_history = " ".join([m.uci() for m in board.move_stack])
        send_command(f"position startpos moves {move_history}")
        
        send_command("go movetime 1000")
        
        while True:
            pygame.event.pump()
            line = engine.stdout.readline().strip()
            
            if line:
                print(line)
                parts = line.split()
                
                if "info" in parts and "depth" in parts:
                    try:
                        depth = parts[parts.index("depth") + 1]
                        seldepth = parts[parts.index("seldepth") + 1] if "seldepth" in parts else "0"
                        current_depth = f"{depth} / {seldepth}"
                        
                        if "nps" in parts:
                            current_nps = int(parts[parts.index("nps") + 1])
                            
                        if "score" in parts:
                            score_idx = parts.index("score")
                            if parts[score_idx + 1] == "cp":
                                raw_score = int(parts[score_idx + 2])
                                if board.turn == chess.BLACK:
                                    raw_score = -raw_score
                                current_eval = raw_score / 100.0
                                current_eval_text = f"{current_eval:+.2f}"
                                
                            elif parts[score_idx + 1] == "mate":
                                mate_in = int(parts[score_idx + 2])
                                current_eval = 10.0 if mate_in > 0 else -10.0
                                if board.turn == chess.BLACK:
                                    current_eval = -current_eval
                                    mate_in = -mate_in
                                current_eval_text = f"M{mate_in}"

                        if "pv" in parts:
                            pv_idx = parts.index("pv")
                            current_pv = " ".join(parts[pv_idx + 1: pv_idx + 5]) # Keep just the first 4 moves

                    except (ValueError, IndexError):
                        pass
                    
                    draw_board(board, selected_sq, valid_moves, last_move, right_clicked_squares, current_eval, current_eval_text, current_depth, current_nps, current_pv)
                    pygame.display.flip()

            if line.startswith("bestmove"):
                best_move_str = line.split()[1]
                break
        
        if best_move_str != "(none)":
            engine_move = chess.Move.from_uci(best_move_str)
            is_capture = board.is_capture(engine_move)
            
            pygame.time.wait(400)
            
            board.push(engine_move)
            last_move = engine_move
            
            if is_capture and capture_sound:
                capture_sound.play()
            elif move_sound:
                move_sound.play()

    if (board.is_game_over() or is_drawn) and not game_over_printed:
        if is_drawn:
            print("Game Over: Draw (Repetition or 50-Move Rule)")
        else:
            print("Game Over:", board.result())
        game_over_printed = True

engine.terminate()
pygame.quit()
sys.exit()