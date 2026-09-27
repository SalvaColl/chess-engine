#include <bits/stdc++.h>
#include "chess.hpp"
#include <chrono>

using namespace std;
using namespace chess;
using namespace std::chrono;

auto start_time = high_resolution_clock::now();
int time_limit_ms = 1000;
bool stop_search = false;
bool infinite_search = false;
long long nodes_visited = 0;

long long tt_cutoffs = 0;
long long nmp_cutoffs = 0;

int max_seldepth = 0;

Board board;
const int INF = 1e9;

const int FLAG_EXACT = 0;
const int FLAG_ALPHA = 1; 
const int FLAG_BETA  = 2;  

const int MAX_PLY = 100;
Move killer_moves[MAX_PLY][2];

int history_table[64][64] = {0};

struct TTEntry {
    uint64_t key = 0;
    int score = 0;
    int depth = -1;
    int flag = 0;
    Move best_move;
};

struct ScoredMove {
    Move move;
    int score;
};

const int TT_SIZE = 1 << 22; 
vector<TTEntry> tt(TT_SIZE);

const int pawn_pst[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};

const int knight_pst[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

const int bishop_pst[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

const int rook_pst[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};

const int queen_pst[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

const int king_pst[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};

const int eg_pawn_pst[64] = {
      0,  0,  0,  0,  0,  0,  0,  0,
     80, 80, 80, 80, 80, 80, 80, 80,
     50, 50, 50, 50, 50, 50, 50, 50,
     30, 30, 30, 30, 30, 30, 30, 30,
     20, 20, 20, 20, 20, 20, 20, 20,
     10, 10, 10, 10, 10, 10, 10, 10,
      0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0
};

const int eg_king_pst[64] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

const int* mg_psts[6] = {pawn_pst, knight_pst, bishop_pst, rook_pst, queen_pst, king_pst};
const int* eg_psts[6] = {eg_pawn_pst, knight_pst, bishop_pst, rook_pst, queen_pst, eg_king_pst};

const int phase_weights[6] = {0, 1, 1, 2, 4, 0};

void send(string msg) {
    cout << msg << "\n" << flush;
}

int piece_value(PieceType pt) {
    if (pt == PieceType::PAWN) return 100;
    if (pt == PieceType::KNIGHT) return 300;
    if (pt == PieceType::BISHOP) return 320;
    if (pt == PieceType::ROOK) return 500;
    if (pt == PieceType::QUEEN) return 900;
    return 0;
}

bool has_non_pawn_material(Color side) {
    uint64_t non_pawns = 
        board.pieces(PieceType::KNIGHT, side).getBits() |
        board.pieces(PieceType::BISHOP, side).getBits() |
        board.pieces(PieceType::ROOK, side).getBits() |
        board.pieces(PieceType::QUEEN, side).getBits();
        
    return non_pawns != 0;
}

int evaluate() {
    int mg_w_score = 0, mg_b_score = 0;
    int eg_w_score = 0, eg_b_score = 0;
    int phase = 0;
    
    PieceType types[] = {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN, PieceType::KING};
    
    for (int i = 0; i < 6; i++) {
        PieceType pt = types[i];
        
        uint64_t w_bits = board.pieces(pt, Color::WHITE).getBits();
        uint64_t b_bits = board.pieces(pt, Color::BLACK).getBits();
        
        int piece_count = __builtin_popcountll(w_bits) + __builtin_popcountll(b_bits);
        phase += phase_weights[i] * piece_count;
        
        while (w_bits) {
            int sq = __builtin_ctzll(w_bits);
            int val = piece_value(pt);
            mg_w_score += val + mg_psts[i][sq ^ 56];
            eg_w_score += val + eg_psts[i][sq ^ 56];
            w_bits &= w_bits - 1;
        }
        
        while (b_bits) {
            int sq = __builtin_ctzll(b_bits);
            int val = piece_value(pt);
            mg_b_score += val + mg_psts[i][sq];
            eg_b_score += val + eg_psts[i][sq];
            b_bits &= b_bits - 1;
        }
    }
    
    if (board.pieces(PieceType::BISHOP, Color::WHITE).count() >= 2) { mg_w_score += 30; eg_w_score += 30; }
    if (board.pieces(PieceType::BISHOP, Color::BLACK).count() >= 2) { mg_b_score += 30; eg_b_score += 30; }
    
    if (phase > 24) phase = 24; 
    
    int mg_eval = mg_w_score - mg_b_score;
    int eg_eval = eg_w_score - eg_b_score;
    
    int eval = (mg_eval * phase + eg_eval * (24 - phase)) / 24;
    
    return (board.sideToMove() == Color::WHITE) ? eval : -eval;
}

int score_move(const Move& move, Move tt_move, int ply) {
    if (move == tt_move) return 1000000;
    
    if (board.isCapture(move)) {
        int attacker_val = 100;
        int victim_val = 100;   
        
        PieceType types[] = {PieceType::PAWN, PieceType::KNIGHT, PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN, PieceType::KING};

        int from_sq = move.from().index();
        for (int i = 0; i < 6; i++) {
            if (board.pieces(types[i], board.sideToMove()).getBits() & (1ULL << from_sq)) {
                attacker_val = piece_value(types[i]);
                break;
            }
        }

        int to_sq = move.to().index();
        Color opp = (board.sideToMove() == Color::WHITE) ? Color::BLACK : Color::WHITE;
        for (int i = 0; i < 6; i++) {
            if (board.pieces(types[i], opp).getBits() & (1ULL << to_sq)) {
                victim_val = piece_value(types[i]);
                break;
            }
        }

        return 100000 + (10 * victim_val - attacker_val);
    }

    if (ply < MAX_PLY) {
        if (move == killer_moves[ply][0]) return 90000;
        if (move == killer_moves[ply][1]) return 80000;
    }

    return history_table[move.from().index()][move.to().index()];
}

vector<Move> order_moves(const Movelist& moves, Move tt_move = Move(), int ply = 0) {
    vector<pair<int, Move>> scored_moves;
    scored_moves.reserve(moves.size());
    
    for (const auto& move : moves) {
        scored_moves.push_back({score_move(move, tt_move, ply), move});
    }
    
    sort(scored_moves.begin(), scored_moves.end(), [](const auto& a, const auto& b) {
        return a.first > b.first;
    });
    
    vector<Move> sorted;
    sorted.reserve(moves.size());
    for (const auto& pair : scored_moves) {
        sorted.push_back(pair.second);
    }
    return sorted;
}

int qsearch(int alpha, int beta, int ply) {
    if (ply > max_seldepth) max_seldepth = ply;

    uint64_t hash_key = board.zobrist();
    TTEntry& tte = tt[hash_key & (TT_SIZE - 1)];
    
    if (tte.key == hash_key) {
        if (tte.flag == FLAG_EXACT) return tte.score;
        if (tte.flag == FLAG_ALPHA && tte.score <= alpha) return alpha;
        if (tte.flag == FLAG_BETA && tte.score >= beta) return beta;
    }

    bool in_check = board.inCheck();
    int stand_pat = -INF;

    if (!in_check) {
        stand_pat = evaluate();
        if (stand_pat >= beta) return beta;
        if (alpha < stand_pat) alpha = stand_pat;

        if (stand_pat + 1000 < alpha) {
            return alpha;
        }
    }

    Movelist moves;
    
    if (in_check) {
        movegen::legalmoves(moves, board);
    } else {
        movegen::legalmoves<movegen::MoveGenType::CAPTURE>(moves, board);
    }
    
    if (ply >= MAX_PLY) return in_check ? 0 : evaluate();
    
    ScoredMove smoves[256];
    int move_count = moves.size();
    for (int i = 0; i < move_count; i++) {
        smoves[i] = {moves[i], score_move(moves[i], Move(), ply)};
    }

    for (int i = 0; i < move_count; i++) {
        int best_idx = i;
        for (int j = i + 1; j < move_count; j++) {
            if (smoves[j].score > smoves[best_idx].score) {
                best_idx = j;
            }
        }
        swap(smoves[i], smoves[best_idx]);
        Move move = smoves[i].move;

        board.makeMove(move);
        int score = -qsearch(-beta, -alpha, ply + 1);
        board.unmakeMove(move);
        
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

int alphabeta(int depth, int alpha, int beta, int ply) {
    if (ply > max_seldepth) max_seldepth = ply;

    if (ply > 0 && (board.isRepetition() || board.isHalfMoveDraw())) {
        return 0; 
    }

    if ((nodes_visited & 2047) == 0) {
        if (stop_search) return 0;
        if (!infinite_search) {
            auto now = high_resolution_clock::now();
            auto elapsed = duration_cast<milliseconds>(now - start_time).count();
            if (elapsed >= time_limit_ms) {
                stop_search = true;
                return 0;
            }
        }
    }

    uint64_t hash_key = board.zobrist();
    TTEntry& tte = tt[hash_key & (TT_SIZE - 1)];
    Move tt_move = Move();

    if (tte.key == hash_key) {
        tt_move = tte.best_move;
        if (tte.depth >= depth && ply > 0) {
            if (tte.flag == FLAG_EXACT) {
                tt_cutoffs++;
                return tte.score;
            }
            if (tte.flag == FLAG_ALPHA && tte.score <= alpha) {
                tt_cutoffs++;
                return alpha;
            }
            if (tte.flag == FLAG_BETA && tte.score >= beta) {
                tt_cutoffs++;
                return beta;
            }
        }
    }

    bool in_check = board.inCheck();

    if (in_check && ply < 16) {
        depth++;
    }

    if (depth <= 0) {
        return qsearch(alpha, beta, ply);
    }

    if (!in_check && depth <= 3 && ply > 0) {
        int rfp_margin = 120 * depth;
        int static_eval = evaluate();
        if (static_eval - rfp_margin >= beta) {
            return static_eval;
        }
    }

    if (!in_check && depth >= 3 && ply > 0 && has_non_pawn_material(board.sideToMove())) {
        int R = 2;
        board.makeNullMove();
        int null_score = -alphabeta(depth - 1 - R, -beta, -beta + 1, ply + 1);
        board.unmakeNullMove();

        if (stop_search) return 0;

        if (null_score >= beta) {
            nmp_cutoffs++;
            return beta;
        }
    }

    Movelist moves;
    movegen::legalmoves(moves, board);

    if (moves.empty()) {
        if (in_check) {
            return -INF + ply; 
        }
        return 0; 
    }

    ScoredMove smoves[256];
    int move_count = moves.size();
    for (int i = 0; i < move_count; i++) {
        smoves[i] = {moves[i], score_move(moves[i], tt_move, ply)};
    }

    int original_alpha = alpha;
    int best_score = -INF;
    Move best_move = Move();
    int moves_searched = 0;

    for (int i = 0; i < move_count; i++) {
        int best_idx = i;
        for (int j = i + 1; j < move_count; j++) {
            if (smoves[j].score > smoves[best_idx].score) {
                best_idx = j;
            }
        }
        swap(smoves[i], smoves[best_idx]);
        Move move = smoves[i].move;

        bool is_capture = board.isCapture(move);

        bool is_killer = false;
        if (ply < MAX_PLY) {
            is_killer = (move == killer_moves[ply][0] || move == killer_moves[ply][1]);
        }

        board.makeMove(move);
        nodes_visited++;
        bool gives_check = board.inCheck();
        int score = 0;

        if (depth <= 3 && moves_searched > (depth * 4) && !is_capture && !in_check && !gives_check && !is_killer) {
            board.unmakeMove(move);
            continue; 
        }

        if (moves_searched == 0) {
            score = -alphabeta(depth - 1, -beta, -alpha, ply + 1);
        } else {
            if (moves_searched >= 3 && depth >= 3 && !is_capture && !in_check && !gives_check && !is_killer) {
                int reduction = (moves_searched >= 6 && depth >= 5) ? 2 : 1;
                
                score = -alphabeta(depth - 1 - reduction, -alpha - 1, -alpha, ply + 1);
                
                if (score > alpha) {
                    score = -alphabeta(depth - 1, -alpha - 1, -alpha, ply + 1);
                }
            } else {
                score = -alphabeta(depth - 1, -alpha - 1, -alpha, ply + 1);
            }

            if (score > alpha && score < beta) {
                score = -alphabeta(depth - 1, -beta, -alpha, ply + 1);
            }
        }

        board.unmakeMove(move);
        moves_searched++;

        if (stop_search) return 0;

        if (score > best_score) {
            best_score = score;
            best_move = move;
        }

        alpha = max(alpha, score);

        if (alpha >= beta) {
            if (!is_capture) {
                history_table[move.from().index()][move.to().index()] += depth * depth;
                
                if (ply < MAX_PLY) {
                    if (killer_moves[ply][0] != move) {
                        killer_moves[ply][1] = killer_moves[ply][0];
                        killer_moves[ply][0] = move;
                    }
                }
            }

            if (tte.key != hash_key || depth >= tte.depth) {
                tte.key = hash_key;
                tte.depth = depth;
                tte.score = beta;
                tte.flag = FLAG_BETA;
                tte.best_move = move;
            }
            return beta;
        }
    }

    if (tte.key != hash_key || depth >= tte.depth) {
        tte.key = hash_key;
        tte.depth = depth;
        tte.score = best_score;
        tte.best_move = best_move;
        tte.flag = (best_score > original_alpha) ? FLAG_EXACT : FLAG_ALPHA;
    }

    return best_score;
}

void handle_go(istringstream& ss) {

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            history_table[i][j] /= 8;
        }
    }
    memset(killer_moves, 0, sizeof(killer_moves));
    max_seldepth = 0;
    tt_cutoffs = 0;
    nmp_cutoffs = 0;

    Movelist moves;
    movegen::legalmoves(moves, board);
    if (moves.empty()) return;

    string token;
    time_limit_ms = 1000; 
    infinite_search = false;

    while (ss >> token) {
        if (token == "movetime") {
            ss >> time_limit_ms;
        } else if (token == "infinite") {
            infinite_search = true;
        }
    }

    start_time = high_resolution_clock::now();
    stop_search = false;
    nodes_visited = 0;

    Move best_move = moves[0];
    auto sorted_root_moves = order_moves(moves);
    int previous_score = 0; 

    for (int current_depth = 1; current_depth <= 64; current_depth++) {
        
        int alpha = -INF;
        int beta = INF;

        if (current_depth >= 3) {
            alpha = max(-INF, previous_score - 50);
            beta = min(INF, previous_score + 50);
        }

        Move iteration_best_move = sorted_root_moves[0];
        int iteration_best_score = -INF;

        while (true) {
            iteration_best_score = -INF;
            int current_alpha = alpha; 

            for (const auto& move : sorted_root_moves) {
                board.makeMove(move);
                int extension = 0;
                if (board.inCheck()) extension = 1;

                int score = -alphabeta(current_depth - 1 + extension, -beta, -current_alpha, 1);
                board.unmakeMove(move);

                if (stop_search) break;

                if (score > iteration_best_score) {
                    iteration_best_score = score;
                    iteration_best_move = move;
                }
                if (score > current_alpha) {
                    current_alpha = score; 
                }
            }

            if (stop_search) break;

            if (iteration_best_score <= alpha) {
                alpha = -INF;
                continue; 
            }
            if (iteration_best_score >= beta) {
                beta = INF;
                continue; 
            }

            break; 
        }

        if (stop_search) break;

        previous_score = iteration_best_score; 
        best_move = iteration_best_move;

        for (size_t i = 0; i < sorted_root_moves.size(); i++) {
            if (sorted_root_moves[i] == best_move) {
                sorted_root_moves.erase(sorted_root_moves.begin() + i);
                sorted_root_moves.insert(sorted_root_moves.begin(), best_move);
                break;
            }
        }

        auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - start_time).count();
        long long safe_elapsed = max(1LL, (long long) elapsed);
        long long nps = nodes_visited * 1000 / safe_elapsed;

        string score_str;
        if (iteration_best_score > INF - 100) {
            int moves_to_mate = (INF - iteration_best_score + 1) / 2;
            score_str = "score mate " + to_string(moves_to_mate);
        } else if (iteration_best_score < -INF + 100) {
            int moves_to_mate = (-INF - iteration_best_score) / 2;
            score_str = "score mate " + to_string(moves_to_mate);
        } else {
            score_str = "score cp " + to_string(iteration_best_score);
        }

        send("info depth " + to_string(current_depth) + 
            " seldepth " + to_string(max_seldepth) + 
            " " + score_str + 
            " nodes " + to_string(nodes_visited) + 
            " time " + to_string(elapsed) + 
            " nps " + to_string(nps) + 
            " pv " + uci::moveToUci(iteration_best_move));

        if (!infinite_search && elapsed >= (time_limit_ms * 0.8)) break;
    }

    send("bestmove " + uci::moveToUci(best_move));
}

void handle_position(istringstream& ss) {
    string token;
    ss >> token;
    
    if (token == "startpos") {
        board.setFen(constants::STARTPOS);
        ss >> token; // Consumes "moves" if present
    } else if (token == "fen") {
        string fen = "";
        while (ss >> token && token != "moves") {
            fen += token + " ";
        }
        board.setFen(fen);
    }
    
    while (ss >> token) {
        Move move = uci::uciToMove(board, token);
        board.makeMove(move);
    }
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    
    string line, command;
    
    while (getline(cin, line)) {
        istringstream ss(line);
        ss >> command;
        
        if (command == "uci") {
            send("id name CP_Engine");
            send("id author You");
            send("uciok");
        } 
        else if (command == "isready") {
            send("readyok");
        } 
        else if (command == "position") {
            handle_position(ss);
        } 
        else if (command == "go") {
            handle_go(ss);
        } 
        else if (command == "quit") {
            break;
        }
    }
    
    return 0;
}