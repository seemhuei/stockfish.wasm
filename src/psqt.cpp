/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2020 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>

#include "types.h"
#include "bitboard.h"

namespace PSQT {

#define S(mg, eg) make_score(mg, eg)

// Bonus[PieceType][Square / 2] contains Piece-Square scores. For each piece
// type on a given square a (middlegame, endgame) score pair is assigned. Table
// is defined for files A..D and white side: it is symmetric for black side and
// second half of the files.
constexpr Score Bonus[VARIANT_NB][PIECE_TYPE_NB][RANK_NB][int(FILE_NB) / 2] = {
  {
    { },
    { },
    { // Knight
     { S(-175, -96), S(-92,-65), S(-74,-49), S(-73,-21) },
     { S( -77, -67), S(-41,-54), S(-27,-18), S(-15,  8) },
     { S( -61, -40), S(-17,-27), S(  6, -8), S( 12, 29) },
     { S( -35, -35), S(  8, -2), S( 40, 13), S( 49, 28) },
     { S( -34, -45), S( 13,-16), S( 44,  9), S( 51, 39) },
     { S(  -9, -51), S( 22,-44), S( 58,-16), S( 53, 17) },
     { S( -67, -69), S(-27,-50), S(  4,-51), S( 37, 12) },
     { S(-201,-100), S(-83,-88), S(-56,-56), S(-26,-17) }
    },
    { // Bishop
     { S(-53,-57), S( -5,-30), S( -8,-37), S(-23,-12) },
     { S(-15,-37), S(  8,-13), S( 19,-17), S(  4,  1) },
     { S( -7,-16), S( 21, -1), S( -5, -2), S( 17, 10) },
     { S( -5,-20), S( 11, -6), S( 25,  0), S( 39, 17) },
     { S(-12,-17), S( 29, -1), S( 22,-14), S( 31, 15) },
     { S(-16,-30), S(  6,  6), S(  1,  4), S( 11,  6) },
     { S(-17,-31), S(-14,-20), S(  5, -1), S(  0,  1) },
     { S(-48,-46), S(  1,-42), S(-14,-37), S(-23,-24) }
    },
    { // Rook
     { S(-31, -9), S(-20,-13), S(-14,-10), S(-5, -9) },
     { S(-21,-12), S(-13, -9), S( -8, -1), S( 6, -2) },
     { S(-25,  6), S(-11, -8), S( -1, -2), S( 3, -6) },
     { S(-13, -6), S( -5,  1), S( -4, -9), S(-6,  7) },
     { S(-27, -5), S(-15,  8), S( -4,  7), S( 3, -6) },
     { S(-22,  6), S( -2,  1), S(  6, -7), S(12, 10) },
     { S( -2,  4), S( 12,  5), S( 16, 20), S(18, -5) },
     { S(-17, 18), S(-19,  0), S( -1, 19), S( 9, 13) }
    },
    { // Queen
     { S( 3,-69), S(-5,-57), S(-5,-47), S( 4,-26) },
     { S(-3,-55), S( 5,-31), S( 8,-22), S(12, -4) },
     { S(-3,-39), S( 6,-18), S(13, -9), S( 7,  3) },
     { S( 4,-23), S( 5, -3), S( 9, 13), S( 8, 24) },
     { S( 0,-29), S(14, -6), S(12,  9), S( 5, 21) },
     { S(-4,-38), S(10,-18), S( 6,-12), S( 8,  1) },
     { S(-5,-50), S( 6,-27), S(10,-24), S( 8, -8) },
     { S(-2,-75), S(-2,-52), S( 1,-43), S(-2,-36) }
    },
    { // King
     { S(271,  1), S(327, 45), S(271, 85), S(198, 76) },
     { S(278, 53), S(303,100), S(234,133), S(179,135) },
     { S(195, 88), S(258,130), S(169,169), S(120,175) },
     { S(164,103), S(190,156), S(138,172), S( 98,172) },
     { S(154, 96), S(179,166), S(105,199), S( 70,199) },
     { S(123, 92), S(145,172), S( 81,184), S( 31,191) },
     { S( 88, 47), S(120,121), S( 65,116), S( 33,131) },
     { S( 59, 11), S( 89, 59), S( 45, 73), S( -1, 78) }
    }
  },
#ifdef ANTI
  {
    { },
    { // Pawn
      { S(  0,  0), S(  0,  0), S(  0,  0), S( 0,  0) },
      { S( 30,-21), S( 13,-12), S( 33, 26), S(21,  4) },
      { S(-27,-58), S(-17,-52), S( 16,-32), S(16,-22) },
      { S( -6,-23), S( -2, 25), S( 35, 24), S(26, 20) },
      { S(-25, 26), S( 18, 27), S( 10, 16), S(31, 50) },
      { S(-22, 19), S(  6,-21), S( -1,  0), S( 9,-11) },
      { S( -2,-30), S( 22,  4), S(-12, 11), S(11, 10) }
    },
    { // Knight
      { S(-150,-106), S(-111, -86), S(-83,-37), S(-91,-20) },
      { S( -81, -52), S( -37, -43), S(-40,  2), S( 10,  7) },
      { S( -65, -22), S(   0, -32), S(  2,  3), S(  3, 49) },
      { S( -49, -32), S(  35, -36), S( 32, -5), S( 40,  5) },
      { S( -14, -70), S(  21,  -5), S( 45, 38), S( 43, 34) },
      { S(   1, -50), S(  38, -67), S( 41,-14), S( 85,  2) },
      { S( -72, -86), S( -39, -81), S( 20,-28), S( 39,  9) },
      { S(-198,-103), S( -96,-100), S(-16,-60), S(-10,-28) }
    },
    { // Bishop
      { S(-73,-53), S(-38,-39), S(-34,-32), S(-27,-54) },
      { S(-33,-54), S( 17,-18), S( -2,-17), S(-12,-14) },
      { S(-29,-45), S(  2,  0), S( 24,  0), S( 11, -9) },
      { S(-21,-32), S( 42,-13), S( -1,-10), S(-14,-14) },
      { S(  0,-18), S( 29,-19), S(-14,-18), S(  2,-15) },
      { S(-32,-46), S( 23, -7), S(  0,  4), S(-18,  9) },
      { S(-28,-32), S( 22,-47), S(  6,-12), S(-12,  7) },
      { S(-61,-77), S( -1,-46), S(-29,-48), S(-47,-24) }
    },
    { // Rook
      { S( -2, 14), S( -4,-49), S(  0,  2), S(-14, -3) },
      { S(-31,  8), S( 15,-18), S( -8,  2), S( 27,  2) },
      { S(-28,-10), S( -2, 17), S(-16,-30), S( -2,  4) },
      { S(-16,-13), S( -4, 13), S(  0,-18), S(  7,-30) },
      { S(-28, 15), S(-20, -6), S(  3,  1), S( 17,  4) },
      { S(-31,  5), S(  1,  0), S(  1, -9), S( -7, 15) },
      { S( 11, -2), S( -1, -5), S( 23, 13), S( 14, 17) },
      { S( -6,  2), S( -9, 31), S( 20,  4), S(  2, -2) }
    },
    { // Queen
      { S(-18,-76), S(-20,-73), S(-13,-15), S( 7,-37) },
      { S(  0,-54), S(  6,-53), S( -1,-21), S(19,-33) },
      { S(-12,-52), S(  8,-15), S( 14,-11), S(10,  8) },
      { S( 13,  7), S(  0, -8), S(-22, 33), S(14,  9) },
      { S( 12,-47), S( 11,-14), S( 12, -7), S(22, 35) },
      { S( 12,-50), S( 17,-12), S( 26,-21), S( 0,  5) },
      { S( 13,-73), S(  6,-28), S( 14,-41), S(-6,  9) },
      { S(-11,-82), S( -4,-68), S( -8,-32), S( 3,-38) }
    },
    { // King
      { S(291, 17), S(292, 75), S(297, 90), S(240,128) },
      { S(269, 69), S(297, 91), S(259,192), S(172,168) },
      { S(245,115), S(267,161), S(191,194), S( 91,158) },
      { S(210,124), S(227,179), S(183,209), S(158,251) },
      { S(155,134), S(187,212), S(128,217), S( 79,249) },
      { S(163, 98), S(193,165), S(102,198), S( 77,205) },
      { S(130, 79), S(163,123), S( 84,128), S( 51,145) },
      { S( 85, 42), S(121, 73), S( 60, 85), S(  4,118) }
    }
  },
#endif
#ifdef ATOMIC
  {
    { },
    { // Pawn
      { S(  0,  0), S(  0,  0), S(  0, 0), S(  0,  0) },
      { S( -4, 16), S( 10,  5), S(-27, 2), S(-11,  2) },
      { S(-32,-10), S( 11,-41), S(  0, 3), S( 26, -4) },
      { S(  7,-22), S( 12, 16), S( -2, 1), S( 25,  9) },
      { S(-26,  1), S( 14, 18), S( 29,14), S( 34, 17) },
      { S( 20,  9), S(-30, -1), S(  7,49), S( 18,-15) },
      { S(-11, -8), S( 27, -6), S( -2,21), S(-20, 29) }
    },
    { // Knight
      { S(-146, -96), S(-162,-112), S(-83,-42), S(-87, -2) },
      { S( -78, -50), S( -24, -63), S( -3,-11), S( 17,  3) },
      { S( -34, -61), S( -34, -70), S(  8,  2), S( 19, 28) },
      { S( -14, -33), S( -21, -27), S( 24,  6), S( 27,  8) },
      { S( -36, -33), S(  15, -14), S( 71,  4), S( 67, 77) },
      { S( -11, -69), S(  15, -38), S( 70,  4), S( 91, 38) },
      { S( -64, -75), S(  -8, -17), S( 29, 18), S( 32, 14) },
      { S(-172,-123), S( -61, -87), S(-37,-17), S( -9,-19) }
    },
    { // Bishop
      { S(-54,-92), S(-38, -9), S(-36,-30), S(-69,-10) },
      { S(-48,-70), S( 17,-26), S(-12,-23), S(-23, -3) },
      { S( -1,-35), S(  9,-11), S(  8,-11), S( 10,  1) },
      { S(-16,-36), S( 16,-29), S(  2,-26), S(  8,-16) },
      { S(-37,-17), S( 22,-30), S( 14,-34), S(-11, -5) },
      { S(-49,-21), S( 37, -6), S(  0, -7), S(-17, -2) },
      { S(-36,-54), S( 20,-11), S(-13,-26), S( -5,  9) },
      { S(-48,-63), S(-10,-25), S(-40, -5), S(-45,-35) }
    },
    { // Rook
      { S(-53,-59), S(-18,-15), S( 24, 21), S(  9,-12) },
      { S(-45,-23), S( 25,-11), S( -7,  3), S(-17,-25) },
      { S(-44,  5), S( -5, -5), S(-21, -5), S( 11,  5) },
      { S(-18,-14), S(-25,-11), S(-28, 10), S( 14, -6) },
      { S(-23,-13), S( -3, -9), S(-19,-25), S(-23,  2) },
      { S( 13, -6), S(-18, -7), S( 12,  8), S( 27,  4) },
      { S( 28, 10), S( 16,-18), S( -9, 15), S( 26, -3) },
      { S(-23, 17), S(-11, -5), S( 13, 26), S( -9,  3) }
    },
    { // Queen
      { S(-36,-68), S( -5,-65), S(-35,-41), S( 37,-32) },
      { S(-27,-71), S( 15,-39), S(  5,-19), S(-20,-22) },
      { S(  1,-26), S( 11,-25), S(-12,-22), S( 17,  0) },
      { S(-20,-39), S( 10,-12), S(  5, 13), S(  4, 17) },
      { S(-26,-15), S( 14, 10), S( 10,  6), S(  3, 24) },
      { S(  4,-49), S( -2,-25), S( -3,-34), S( -5, 16) },
      { S( -6,-40), S(  0,-33), S( 31,-14), S(  1, -7) },
      { S( -5,-82), S(-16,-58), S(  9,-35), S(  3,-34) }
    },
    { // King
      { S(272, 22), S(367, 81), S(257, 71), S(169, 45) },
      { S(242, 47), S(290,116), S(199,125), S(157,157) },
      { S(176, 60), S(237,117), S(166,167), S(116,133) },
      { S(186, 98), S(183,158), S(136,164), S(110,204) },
      { S(157, 68), S(177,134), S( 96,180), S( 41,203) },
      { S(121,106), S(172,133), S( 84,160), S( 41,200) },
      { S( 90, 71), S(128,101), S( 68, 89), S(  2,139) },
      { S( 48,  9), S(109, 58), S( 31, 94), S( 18, 66) }
    }
  },
#endif
#ifdef CRAZYHOUSE
  {
    { },
    { // Pawn
      { S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0) },
      { S( -9,  0), S( 29,-22), S( 17, 28), S(-28,  6) },
      { S(-14,-50), S(-25, 14), S( 51, 34), S( 18, 21) },
      { S(-64, 16), S(-60, 13), S( 16, -5), S(-19,-30) },
      { S( 35, 26), S( 11, 14), S( 16,-86), S(111,  5) },
      { S( 27,  7), S(-24,-27), S( 12,-41), S(  9, 32) },
      { S(  8, 51), S( 28,-66), S(-40,-38), S( 64,  2) }
    },
    { // Knight
      { S(-145,-132), S(-276, -59), S( -98,   4), S( -32,-30) },
      { S( -97,-129), S( -47,-129), S( -56,  16), S( -46, 82) },
      { S( -54, -71), S( -61, -26), S(   7,  65), S( -47,-58) },
      { S( -23, -53), S(  92, -33), S(  74,  40), S(  89, 36) },
      { S( 121, -72), S(   0, -99), S(  -9,  63), S( 121,106) },
      { S(  35, -11), S(  46,-126), S(  20, -56), S(  50, 26) },
      { S( -87, -30), S( -32, -25), S( -20, -72), S(  44,-49) },
      { S(-189, -98), S(-133, -78), S( -77, -87), S(  98, 64) }
    },
    { // Bishop
      { S( -9,-65), S(-48,-71), S(-78,-36), S(-21,  9) },
      { S(  8, -9), S( 50, 12), S( 38, -7), S( 28,-23) },
      { S( 35, 15), S( 38,-43), S( 92,-19), S( 51, 28) },
      { S(  3,-33), S( 56,-30), S( 60,-31), S( -4, 52) },
      { S( 27,-34), S( 34,-10), S( 23,-75), S( 13,  9) },
      { S(-15,-54), S(  3,  2), S( -7,-22), S(-50,-26) },
      { S(-34,-34), S( 33, 22), S(-17,-15), S( 15, 33) },
      { S(-34,-23), S(  2,-63), S(-17,-22), S( -3, -4) }
    },
    { // Rook
      { S(-50, 14), S(  4,-33), S( -7, 30), S(-29, 10) },
      { S(-65,  0), S( 18, -2), S( 17,  0), S( 23, 23) },
      { S(-23,-38), S(-51, 13), S( 19,  1), S(  4,  4) },
      { S( -5, 39), S( 15, 26), S(-22,-16), S(-50, 23) },
      { S(-80, 23), S(-53,-29), S(-30, 18), S(  0,-36) },
      { S(  8, 71), S(  8,-20), S( 21, -3), S(  3, 25) },
      { S( 42, 21), S( -8,-20), S(  7, -1), S(  0,-65) },
      { S(-28, 20), S(-22, 11), S(-61,-19), S(-28, 52) }
    },
    { // Queen
      { S( 22,-68), S(  7,-75), S( 13,-54), S( 41,-54) },
      { S( -2,-65), S(  0,-28), S( -3,-17), S( 33,  8) },
      { S( 18,-35), S(  0,-13), S(-23,-60), S(-40,-17) },
      { S( -2,-86), S( -5,  5), S(-17,-41), S(-26,-20) },
      { S(-56,-40), S(-36, 18), S(-36, 21), S(-11, 19) },
      { S(-39,-65), S(  9, 24), S(-33, -7), S(  3, 27) },
      { S( -4,-25), S(-31,-14), S(-19,-81), S(-18,-34) },
      { S(  0,-62), S( 80,-47), S( 37,-68), S( 42, 22) }
    },
    { // King
      { S(354,221), S(463,424), S(192, 61), S( 49, 99) },
      { S(236,127), S(321,121), S(257, 70), S(198,-18) },
      { S(221, 55), S(229, 82), S(270,113), S(150, 74) },
      { S(267,138), S(239,161), S(190,216), S(243,150) },
      { S(164,118), S(224,175), S(155,201), S( 97,267) },
      { S(105,140), S(176,173), S(103,187), S( 90,225) },
      { S(131, 70), S(156,122), S(104,178), S( 30,123) },
      { S(124,-17), S(100, 36), S( 89,107), S( 69,131) }
    }
  },
#endif
#ifdef EXTINCTION
  {
    { },
    { // Pawn
      { S(  0,  0), S(  0,  0), S(  0,  0), S( 0,  0) },
      { S( 30,-21), S( 13,-12), S( 33, 26), S(21,  4) },
      { S(-27,-58), S(-17,-52), S( 16,-32), S(16,-22) },
      { S( -6,-23), S( -2, 25), S( 35, 24), S(26, 20) },
      { S(-25, 26), S( 18, 27), S( 10, 16), S(31, 50) },
      { S(-22, 19), S(  6,-21), S( -1,  0), S( 9,-11) },
      { S( -2,-30), S( 22,  4), S(-12, 11), S(11, 10) }
    },
    { // Knight
      { S(-150,-106), S(-111, -86), S(-83,-37), S(-91,-20) },
      { S( -81, -52), S( -37, -43), S(-40,  2), S( 10,  7) },
      { S( -65, -22), S(   0, -32), S(  2,  3), S(  3, 49) },
      { S( -49, -32), S(  35, -36), S( 32, -5), S( 40,  5) },
      { S( -14, -70), S(  21,  -5), S( 45, 38), S( 43, 34) },
      { S(   1, -50), S(  38, -67), S( 41,-14), S( 85,  2) },
      { S( -72, -86), S( -39, -81), S( 20,-28), S( 39,  9) },
      { S(-198,-103), S( -96,-100), S(-16,-60), S(-10,-28) }
    },
    { // Bishop
      { S(-73,-53), S(-38,-39), S(-34,-32), S(-27,-54) },
      { S(-33,-54), S( 17,-18), S( -2,-17), S(-12,-14) },
      { S(-29,-45), S(  2,  0), S( 24,  0), S( 11, -9) },
      { S(-21,-32), S( 42,-13), S( -1,-10), S(-14,-14) },
      { S(  0,-18), S( 29,-19), S(-14,-18), S(  2,-15) },
      { S(-32,-46), S( 23, -7), S(  0,  4), S(-18,  9) },
      { S(-28,-32), S( 22,-47), S(  6,-12), S(-12,  7) },
      { S(-61,-77), S( -1,-46), S(-29,-48), S(-47,-24) }
    },
    { // Rook
      { S( -2, 14), S( -4,-49), S(  0,  2), S(-14, -3) },
      { S(-31,  8), S( 15,-18), S( -8,  2), S( 27,  2) },
      { S(-28,-10), S( -2, 17), S(-16,-30), S( -2,  4) },
      { S(-16,-13), S( -4, 13), S(  0,-18), S(  7,-30) },
      { S(-28, 15), S(-20, -6), S(  3,  1), S( 17,  4) },
      { S(-31,  5), S(  1,  0), S(  1, -9), S( -7, 15) },
      { S( 11, -2), S( -1, -5), S( 23, 13), S( 14, 17) },
      { S( -6,  2), S( -9, 31), S( 20,  4), S(  2, -2) }
    },
    { // Queen
      { S(-18,-76), S(-20,-73), S(-13,-15), S( 7,-37) },
      { S(  0,-54), S(  6,-53), S( -1,-21), S(19,-33) },
      { S(-12,-52), S(  8,-15), S( 14,-11), S(10,  8) },
      { S( 13,  7), S(  0, -8), S(-22, 33), S(14,  9) },
      { S( 12,-47), S( 11,-14), S( 12, -7), S(22, 35) },
      { S( 12,-50), S( 17,-12), S( 26,-21), S( 0,  5) },
      { S( 13,-73), S(  6,-28), S( 14,-41), S(-6,  9) },
      { S(-11,-82), S( -4,-68), S( -8,-32), S( 3,-38) }
    },
    { // King
      { S(291, 17), S(292, 75), S(297, 90), S(240,128) },
      { S(269, 69), S(297, 91), S(259,192), S(172,168) },
      { S(245,115), S(267,161), S(191,194), S( 91,158) },
      { S(210,124), S(227,179), S(183,209), S(158,251) },
      { S(155,134), S(187,212), S(128,217), S( 79,249) },
      { S(163, 98), S(193,165), S(102,198), S( 77,205) },
      { S(130, 79), S(163,123), S( 84,128), S( 51,145) },
      { S( 85, 42), S(121, 73), S( 60, 85), S(  4,118) }
    }
  },
#endif
#ifdef GRID
  {
    { },
    { // Pawn
     { S(  0, 0), S(  0, 0), S(  0, 0), S( 0, 0) },
     { S(-11, 7), S(  6,-4), S(  7, 8), S( 3,-2) },
     { S(-18,-4), S( -2,-5), S( 19, 5), S(24, 4) },
     { S(-17, 3), S( -9, 3), S( 20,-8), S(35,-3) },
     { S( -6, 8), S(  5, 9), S(  3, 7), S(21,-6) },
     { S( -6, 8), S( -8,-5), S( -6, 2), S(-2, 4) },
     { S( -4, 3), S( 20,-9), S( -8, 1), S(-4,18) }
    },
    { // Knight
     { S(-161,-105), S(-96,-82), S(-80,-46), S(-73,-14) },
     { S( -83, -69), S(-43,-54), S(-21,-17), S(-10,  9) },
     { S( -71, -50), S(-22,-39), S(  0, -7), S(  9, 28) },
     { S( -25, -41), S( 18,-25), S( 43,  6), S( 47, 38) },
     { S( -26, -46), S( 16,-25), S( 38,  3), S( 50, 40) },
     { S( -11, -54), S( 37,-38), S( 56, -7), S( 65, 27) },
     { S( -63, -65), S(-19,-50), S(  5,-24), S( 14, 13) },
     { S(-195,-109), S(-67,-89), S(-42,-50), S(-29,-13) }
    },
    { // Bishop
     { S(-44,-58), S(-13,-31), S(-25,-37), S(-34,-19) },
     { S(-20,-34), S( 20, -9), S( 12,-14), S(  1,  4) },
     { S( -9,-23), S( 27,  0), S( 21, -3), S( 11, 16) },
     { S(-11,-26), S( 28, -3), S( 21, -5), S( 10, 16) },
     { S(-11,-26), S( 27, -4), S( 16, -7), S(  9, 14) },
     { S(-17,-24), S( 16, -2), S( 12,  0), S(  2, 13) },
     { S(-23,-34), S( 17,-10), S(  6,-12), S( -2,  6) },
     { S(-35,-55), S(-11,-32), S(-19,-36), S(-29,-17) }
    },
    { // Rook
     { S(-25, 0), S(-16, 0), S(-16, 0), S(-9, 0) },
     { S(-21, 0), S( -8, 0), S( -3, 0), S( 0, 0) },
     { S(-21, 0), S( -9, 0), S( -4, 0), S( 2, 0) },
     { S(-22, 0), S( -6, 0), S( -1, 0), S( 2, 0) },
     { S(-22, 0), S( -7, 0), S(  0, 0), S( 1, 0) },
     { S(-21, 0), S( -7, 0), S(  0, 0), S( 2, 0) },
     { S(-12, 0), S(  4, 0), S(  8, 0), S(12, 0) },
     { S(-23, 0), S(-15, 0), S(-11, 0), S(-5, 0) }
    },
    { // Queen
     { S( 0,-71), S(-4,-56), S(-3,-42), S(-1,-29) },
     { S(-4,-56), S( 6,-30), S( 9,-21), S( 8, -5) },
     { S(-2,-39), S( 6,-17), S( 9, -8), S( 9,  5) },
     { S(-1,-29), S( 8, -5), S(10,  9), S( 7, 19) },
     { S(-3,-27), S( 9, -5), S( 8, 10), S( 7, 21) },
     { S(-2,-40), S( 6,-16), S( 8,-10), S(10,  3) },
     { S(-2,-55), S( 7,-30), S( 7,-21), S( 6, -6) },
     { S(-1,-74), S(-4,-55), S(-1,-43), S( 0,-30) }
    },
    { // King
     { S(267,  0), S(320, 48), S(270, 75), S(195, 84) },
     { S(264, 43), S(304, 92), S(238,143), S(180,132) },
     { S(200, 83), S(245,138), S(176,167), S(110,165) },
     { S(177,106), S(185,169), S(148,169), S(110,179) },
     { S(149,108), S(177,163), S(115,200), S( 66,203) },
     { S(118, 95), S(159,155), S( 84,176), S( 41,174) },
     { S( 87, 50), S(128, 99), S( 63,122), S( 20,139) },
     { S( 63,  9), S( 88, 55), S( 47, 80), S(  0, 90) }
    }
  },
#endif
#ifdef HORDE
  {
    { },
    { // Pawn
      { S( -24,-99), S(-40,-115), S(-46, -63), S( 69,  -8) },
      { S( -48,-57), S( 40, -32), S(-24, -26), S( 41, -20) },
      { S(-120,-62), S( 25, -19), S(-18, -19), S( 15,  17) },
      { S( -30,-30), S( 42,  23), S(  0,   3), S( 76,  26) },
      { S( -14,-30), S( 50,   4), S( -9,  44), S( 33,  52) },
      { S(  -1, 29), S(-11,   1), S( 30,  47), S( 28,  56) },
      { S(   1, 18), S( 33,  55), S( 63,  29), S( 31,  20) },
    },
    { // Knight
      { S(-131, -94), S(-95, -77), S(-41,-26), S(-66, -8) },
      { S(-108, -77), S(-42, -61), S(-11,-58), S( 13, 10) },
      { S( -70, -59), S(-10, -40), S(-15,-19), S(  9, 23) },
      { S( -43, -38), S(  9, -60), S( 54, 20), S( 55, 31) },
      { S( -31, -43), S( -2, -12), S( 21, -5), S( 53, 22) },
      { S(   6, -60), S( 72, -37), S( 34,-11), S( 77,  7) },
      { S( -53, -68), S(-18, -50), S( 11,-46), S( 11,  2) },
      { S(-204,-131), S(-50,-112), S(-29,-47), S( -6,-19) }
    },
    { // Bishop
      { S(-53,-49), S(-24,-54), S( -2,-46), S(-25,-14) },
      { S(-41,-61), S(  7,-12), S(  5, -9), S(-38,-21) },
      { S(-45,-35), S( -3, -1), S(  2,  6), S(-19, 14) },
      { S(-40,-51), S( 31, -9), S( 51,  4), S(  7,-19) },
      { S(  2,-46), S(  5, -1), S(  4,-43), S( 11, 19) },
      { S( -9,-61), S(-10, -7), S(-12,-17), S( -6, 11) },
      { S(-35,-26), S( 28,-41), S( 16,-39), S( -8, -5) },
      { S(-53,-67), S(-31,-53), S(-29,-67), S(-14,  4) }
    },
    { // Rook
      { S(-23, 20), S(-46, 23), S(-41,-16), S(-32,-19) },
      { S(-52, 21), S(-32, -1), S(-21,-12), S( 11, 14) },
      { S( 11, 15), S(-12,-13), S( -8,-16), S( -8,-25) },
      { S( 18, 48), S(-21, -2), S( 30,-33), S(-21,  6) },
      { S( -3, 32), S( -1,-18), S(-11, -2), S(  3,-29) },
      { S(  0,  0), S(-14,-17), S( -4,-19), S(  6,-15) },
      { S(  2,-19), S(-27, -1), S(-31, 21), S( 30, -5) },
      { S(  0,-33), S(-38, 16), S(-27,-24), S( -2, 20) }
    },
    { // Queen
      { S(  5,-55), S( -1,-65), S(-19,-40), S(  6,-26) },
      { S( 24,-58), S( 11, -9), S(  9,-28), S(-26,-17) },
      { S( 12,-45), S(-21,-15), S( -4, 19), S(  0,  4) },
      { S( 17,-54), S( 18,  4), S( 22,-11), S( -5, 10) },
      { S( 17,  1), S( 28,-16), S(-12,  5), S( 15, 10) },
      { S(-16,-21), S( -8,-20), S(  5, -9), S( 42, 13) },
      { S(-10,-29), S( 12,-35), S(  3,-30), S(-25, -6) },
      { S( 14,-31), S(-10,-44), S( 11,-63), S( -7,-21) }
    },
    { // King
      { S(156, 72), S(381, 37), S(398, 90), S(488,103) },
      { S(226, 36), S(305,143), S(327,177), S(267,234) },
      { S(285,  0), S(266,121), S(254, 61), S(178,172) },
      { S(230,173), S(308,233), S(158,230), S(-44, 68) },
      { S(146,106), S(268,186), S(156,236), S( 11,-67) },
      { S(211,142), S(202,166), S( 86,165), S(170,275) },
      { S( 41,-24), S(205,103), S(216,136), S(-54,123) },
      { S(105, 58), S(207,-19), S( 75, 59), S(-26, 57) }
    }
  },
#endif
#ifdef KOTH
  {
    { },
    { // Pawn
      { S(  0,  0), S(  0,  0), S(  0, 0), S(  0,  0) },
      { S(  5,  5), S( -1,-21), S(-25, 4), S(-33,-32) },
      { S(-27,-14), S( 25,  3), S( 26,-5), S(-19,-11) },
      { S(-27, -3), S( 32,-10), S( 61,17), S( 91,  9) },
      { S(-28, -5), S( 12, 30), S( 35,41), S( 76,  0) },
      { S( -7, 26), S(-18, -8), S(-17,-5), S( 24, 12) },
      { S( 20, 44), S( 24,-20), S( 12,41), S(  9, 14) }
    },
    { // Knight
      { S(-128, -93), S(-139,-110), S(-89,-33), S(-74,  3) },
      { S( -91, -53), S( -63, -73), S(-26,-15), S(-10,-23) },
      { S( -59, -41), S( -14, -36), S( 17,-32), S( 21, 49) },
      { S(   0, -55), S(   2, -31), S( 32, 13), S( 87, 45) },
      { S( -46, -35), S(  35, -12), S( 43, 16), S( 86, 39) },
      { S( -26, -13), S(  23, -88), S( 63,  3), S( 87, 41) },
      { S( -99, -79), S( -15, -25), S(  7,-24), S(  2, 17) },
      { S(-194,-139), S( -43,-114), S(-62,-25), S(-32,-22) }
    },
    { // Bishop
      { S(-16,-31), S( -9,-30), S(-59,-72), S(-38, -9) },
      { S(-40,-49), S( 37,-52), S( -3, -9), S(-13,-11) },
      { S(-26, -7), S( 27, 25), S( 18,-34), S( 14,-10) },
      { S(-10,-29), S( 25,-27), S( 37,-25), S(  5, 16) },
      { S(-42,-28), S( 18,-54), S( 39,-21), S(-12,  9) },
      { S(-45,-35), S( 31,-13), S( 10,-18), S(-12,-12) },
      { S(-43,-36), S( -8,-37), S(-19, -8), S(-16, 27) },
      { S(-97,-61), S(-21,-29), S(-23,-69), S(-43,  5) }
    },
    { // Rook
      { S(-57,-20), S(-12,  3), S( 36, -5), S( 41,  0) },
      { S(-20, 33), S(  0, 66), S( -3, 11), S( 20,-43) },
      { S(-52,  1), S( -9, 44), S(-17,-14), S( 40,-11) },
      { S(-11,  0), S(  2,-26), S(  7,  2), S( 35, -8) },
      { S(-14, 20), S(-14,  6), S( 11,-33), S( 29,-18) },
      { S(-23,-38), S( 33, 10), S(-12,-15), S(-16,-13) },
      { S(  0,-13), S(-17, 29), S( 17,  7), S( 26,-26) },
      { S(-27,  7), S(-34, 12), S(-23,-21), S(  1, -5) }
    },
    { // Queen
      { S(-32,-57), S( 12,-84), S(-17,-27), S(  8,-34) },
      { S(-17,-43), S(-22,-28), S( -3,-27), S( 22,  4) },
      { S( -7,-73), S( 36,-16), S( 27,  1), S(  9, -9) },
      { S(-17, -2), S( 25, 16), S( 10,-25), S( 21, 56) },
      { S( -8,-25), S(-20, 19), S( 15, 18), S(  7, 31) },
      { S( 36,-28), S( 25,-49), S( 24,  0), S( 10, 20) },
      { S( 37,-48), S(  3,-25), S(  2,-25), S(-10, 23) },
      { S(-43,-68), S(  5,-39), S(-14,-34), S( 13,-29) }
    },
    { // King
      { S(209, 35), S(379,-31), S(386,187), S(361, 65) },
      { S(256, 26), S(294, 74), S(329,235), S(306,430) },
      { S(253, 37), S(297,109), S(269, 86), S(280,191) },
      { S(246,169), S(253,242), S(168,217), S(-21, 25) },
      { S(140,130), S(206,197), S(140,221), S(-11,-16) },
      { S(197,107), S(181,213), S(140,152), S(195,270) },
      { S( 82,  1), S(158,149), S( 75,131), S(-58,126) },
      { S(158, 47), S(176, -3), S( 64, 82), S( 59, 36) }
    }
  },
#endif
#ifdef LOSERS
  {
    { },
    { // Pawn
      { S(  0, 0), S(  0, 0), S(  0, 0), S( 0, 0) },
      { S(-11, 7), S(  6,-4), S(  7, 8), S( 3,-2) },
      { S(-18,-4), S( -2,-5), S( 19, 5), S(24, 4) },
      { S(-17, 3), S( -9, 3), S( 20,-8), S(35,-3) },
      { S( -6, 8), S(  5, 9), S(  3, 7), S(21,-6) },
      { S( -6, 8), S( -8,-5), S( -6, 2), S(-2, 4) },
      { S( -4, 3), S( 20,-9), S( -8, 1), S(-4,18) }
    },
    { // Knight
      { S(-143, -97), S(-96,-82), S(-80,-46), S(-73,-14) },
      { S( -83, -69), S(-43,-55), S(-21,-17), S(-10,  9) },
      { S( -71, -50), S(-22,-39), S(  0, -8), S(  9, 28) },
      { S( -25, -41), S( 18,-25), S( 43,  7), S( 47, 38) },
      { S( -26, -46), S( 16,-25), S( 38,  2), S( 50, 41) },
      { S( -11, -55), S( 37,-38), S( 56, -8), S( 71, 27) },
      { S( -62, -64), S(-17,-50), S(  5,-24), S( 14, 13) },
      { S(-195,-110), S(-66,-90), S(-42,-50), S(-29,-13) }
    },
    { // Bishop
      { S(-54,-68), S(-23,-40), S(-35,-46), S(-44,-28) },
      { S(-30,-43), S( 10,-17), S(  2,-23), S( -9, -5) },
      { S(-19,-32), S( 17, -9), S( 11,-13), S(  1,  8) },
      { S(-21,-36), S( 18,-13), S( 11,-15), S(  0,  7) },
      { S(-21,-36), S( 14,-14), S(  6,-17), S( -1,  3) },
      { S(-27,-35), S(  6,-13), S(  2,-10), S( -8,  1) },
      { S(-33,-44), S(  7,-21), S( -4,-22), S(-12, -4) },
      { S(-45,-65), S(-21,-42), S(-29,-46), S(-39,-27) }
    },
    { // Rook
      { S(-25, 0), S(-16, 0), S(-16, 0), S(-9, 0) },
      { S(-21, 0), S( -8, 0), S( -3, 0), S( 0, 0) },
      { S(-21, 0), S( -9, 0), S( -4, 0), S( 2, 0) },
      { S(-22, 0), S( -6, 0), S( -1, 0), S( 2, 0) },
      { S(-22, 0), S( -7, 0), S(  0, 0), S( 1, 0) },
      { S(-21, 0), S( -7, 0), S(  0, 0), S( 2, 0) },
      { S(-12, 0), S(  4, 0), S(  8, 0), S(12, 0) },
      { S(-23, 0), S(-15, 0), S(-11, 0), S(-5, 0) }
    },
    { // Queen
      { S( 0,-70), S(-3,-57), S(-4,-41), S(-1,-29) },
      { S(-4,-58), S( 6,-30), S( 9,-21), S( 8, -4) },
      { S(-2,-39), S( 6,-17), S( 9, -7), S( 9,  5) },
      { S(-1,-29), S( 8, -5), S(10,  9), S( 7, 17) },
      { S(-3,-27), S( 9, -5), S( 8, 10), S( 7, 23) },
      { S(-2,-40), S( 6,-16), S( 8,-11), S(10,  3) },
      { S(-2,-54), S( 7,-30), S( 7,-21), S( 6, -7) },
      { S(-1,-75), S(-4,-54), S(-1,-44), S( 0,-30) }
    },
    { // King
      { S(260,  0), S(313, 48), S(263, 75), S(188, 84) },
      { S(258, 42), S(298, 91), S(232,142), S(174,131) },
      { S(195, 81), S(240,136), S(171,167), S(105,163) },
      { S(173,103), S(181,166), S(144,166), S(106,176) },
      { S(146,104), S(174,159), S(112,196), S( 63,199) },
      { S(116, 90), S(157,150), S( 82,171), S( 39,169) },
      { S( 85, 44), S(127, 93), S( 62,114), S( 17,133) },
      { S( 63,  2), S( 89, 48), S( 47, 73), S(  0, 83) }
    }
  },
#endif
#ifdef RACE
  {
    { },
    { // Pawn
    },
    { // Knight
      { S(-232,-133), S(-45,-190), S(-181,-198), S(-111,-123) },
      { S( -76, -64), S(-22, -88), S(   9, -29), S( -37, -74) },
      { S(  50, -28), S( 16, -35), S(  26, -39), S(  84,  27) },
      { S(  23,  21), S( 63, -37), S(  96,  69), S( 176, 130) },
      { S(  65,  17), S( 62,   4), S( 101, 150), S( 142, 170) },
      { S(  36,  40), S( 18,  54), S( 100,  64), S( 187, 161) },
      { S( -44,  43), S( 71,  57), S(  22, 144), S(  22,  64) },
      { S( -51, -39), S(-12, -26), S( -22,  52), S( -12, 143) }
    },
    { // Bishop
      { S(-48,-18), S(-14,  7), S(-31,-50), S(-53,-37) },
      { S(-37,-11), S( 32,  0), S( 14,-15), S( 35, 30) },
      { S(-40, 27), S( 48,-40), S( 20,  5), S( 21, 10) },
      { S(  1,-45), S( 45, 23), S(-27,  6), S( 78,  4) },
      { S( -2,  2), S( 32, 26), S( 54,  4), S( 46, 71) },
      { S( 24,-18), S( 60, 31), S( 56, 29), S( 51, 50) },
      { S( 54, 58), S( 83, 83), S( 66, 34), S( 63, 34) },
      { S( -3,-12), S( 48, 66), S( -5, 29), S( 53, 58) }
    },
    { // Rook
      { S(-40, 1), S(  8,-12), S(-16, 27), S(  6, 61) },
      { S(18, 57), S(-43,  3), S( 51, -6), S( 23, 27) },
      { S( 0, 13), S( 59,  0), S( 34, 11), S( 29, 41) },
      { S( 7, 38), S( 37, 25), S( 51, 35), S( 54, 27) },
      { S(22, 26), S( 63, 30), S( 59, 28), S( 37, 82) },
      { S(42, 49), S( 24, 54), S( 62,115), S( 81, 31) },
      { S(27, 34), S( 16, 55), S( 39, 55), S(113,108) },
      { S(41, 51), S( 38, 78), S( 53, 79), S( 18,111) }
    },
    { // Queen
      { S(96,-46), S(13,-24), S( 23,-27), S(-19,-31) },
      { S( 8,-28), S(43, -4), S( 19, -8), S( 20, 42) },
      { S(15, -3), S(49, -1), S( 58,  0), S( -1, 37) },
      { S(39, 49), S(66,  7), S(-10, 44), S( 32, 62) },
      { S(53, 10), S(44, 46), S( 24, 40), S( 24, 31) },
      { S(43, 20), S(58,  9), S(108, 45), S( 45, 37) },
      { S(54,-29), S(50, 28), S( 76, 57), S( 61, 30) },
      { S(41,-35), S(45, 37), S( 62,-14), S( 63, 56) }
    },
    { // King
      { S( 85, -17), S(116, 28), S( 58, 57), S(  2, 83) },
      { S( 78,  79), S(148,107), S( 94,162), S( 38,112) },
      { S(150, 114), S(154,198), S( 79,196), S( 90,205) },
      { S(110, 127), S(221,148), S(174,238), S( 82,203) },
      { S(157,  88), S(207,206), S(255,218), S(196,230) },
      { S(223,  96), S(293,211), S(244,238), S(184,269) },
      { S(273,  73), S(377,156), S(298,197), S(246,197) },
      { S(348,  38), S(414,146), S(321,155), S(290,149) }
    }
  },
#endif
#ifdef THREECHECK
  {
    { },
    { // Pawn
      { S(  0,  0), S(  0,  0), S(  0, 0), S(  0,  0) },
      { S( 28,  5), S( 14, 14), S( 48,42), S( -3,-17) },
      { S(-36, -7), S(-47,-35), S( 23,-8), S(-19, -1) },
      { S(-24,  5), S(-38,-16), S(  0, 0), S( 29,-13) },
      { S(-23, 14), S(  4, 30), S( -4,25), S( 49, 37) },
      { S(-16, 12), S(  1, -2), S(-11,-2), S(-27,-13) },
      { S(  1,-12), S( 11,  7), S(-11, 6), S(-45, 32) }
    },
    { // Knight
      { S(-132, -94), S(-145, -51), S(-76, -3), S(-107,-21) },
      { S( -86, -84), S( -37, -53), S(  4,-21), S(  -7, 53) },
      { S( -43, -52), S( -35, -21), S( 20, -4), S(  18, 39) },
      { S( -28, -14), S(  48, -26), S( 52, 29), S(  60, 48) },
      { S( -23, -51), S(  11, -34), S( 13, -3), S(  88, 33) },
      { S( -12, -44), S(  51,  -3), S( 63, -4), S(  78, 31) },
      { S( -85, -71), S( -41, -78), S(  2,-27), S(   7, 24) },
      { S(-148, -88), S( -95,-126), S(-39,-12), S( -21,-21) }
    },
    { // Bishop
      { S(  1,-45), S(-23,-42), S(-106,-43), S(-42,-28) },
      { S(-17,-41), S( 47,-14), S( -20,-49), S(-11,-15) },
      { S(-33,-10), S( 22,-13), S(  -2, -5), S( 33, 28) },
      { S(-30,-37), S( -8, -9), S(  42,  2), S( 26, -4) },
      { S(-54,-37), S(  0, -7), S(  25,-18), S( -1, 15) },
      { S(-42,-11), S(-26,  0), S(   0,-16), S(  0,-18) },
      { S(-25,-71), S(  8,-10), S( -29,-22), S( -2, -5) },
      { S(-28,-76), S(-34,-50), S( -25,-50), S(-52,-29) }
    },
    { // Rook
      { S(-94,-23), S(-21,  4), S( -1, -5), S( 29,-13) },
      { S(-33, -7), S( -5, -3), S(  4, 18), S( 15, -1) },
      { S(  1, 34), S( -6, 19), S( -8,-15), S(-22, 11) },
      { S(-11,-32), S( 22, 10), S(  3,  5), S( 11,-44) },
      { S(-28, -2), S(-28, -7), S(-25,  0), S( -8, -5) },
      { S(  2, 35), S(-26,-31), S( 46, 21), S( 31, 19) },
      { S(-22,-42), S(-22, 11), S( 14,  1), S( 41, 26) },
      { S(-10,-23), S(-20,-14), S( 28, 19), S(  3, 30) }
    },
    { // Queen
      { S(-16,-88), S(11,-47), S(-15,-45), S( 22,-49) },
      { S(  7,-48), S(14,-42), S( 11,-24), S(  2, -9) },
      { S(  0,-20), S(11,-15), S(  0, -3), S( 10,  6) },
      { S(-11,-19), S(25, 10), S( -4, -8), S( 22,  2) },
      { S( -8,-20), S( 0, 16), S( 12,  0), S(  1, 22) },
      { S( 26,-39), S(-8, 15), S( 21,-27), S( 13,  0) },
      { S( 21,-37), S(14,-38), S(-25,-14), S(-24, 12) },
      { S(-22,-82), S(12, -8), S(-12,-24), S(-35,-11) }
    },
    { // King
      { S(310, -9), S(360, 77), S(265, 55), S(153, 41) },
      { S(249, 33), S(319,123), S(218,146), S(206,147) },
      { S(194, 69), S(246,139), S(184,184), S( 87,143) },
      { S(193, 89), S(181,190), S(141,154), S( 95,186) },
      { S(152,116), S(166,156), S(103,185), S( 89,202) },
      { S(116, 51), S(191,145), S( 76,159), S( 53,177) },
      { S( 91, 56), S(110,102), S( 50,106), S(  8,145) },
      { S( 80,-36), S( 74, 40), S( 12, 78), S(-11,103) }
    }
  },
#endif
#ifdef TWOKINGS
  {
    { },
    { // Pawn
     { S(  0, 0), S(  0, 0), S(  0, 0), S( 0, 0) },
     { S(-11, 7), S(  6,-4), S(  7, 8), S( 3,-2) },
     { S(-18,-4), S( -2,-5), S( 19, 5), S(24, 4) },
     { S(-17, 3), S( -9, 3), S( 20,-8), S(35,-3) },
     { S( -6, 8), S(  5, 9), S(  3, 7), S(21,-6) },
     { S( -6, 8), S( -8,-5), S( -6, 2), S(-2, 4) },
     { S( -4, 3), S( 20,-9), S( -8, 1), S(-4,18) }
    },
    { // Knight
     { S(-161,-105), S(-96,-82), S(-80,-46), S(-73,-14) },
     { S( -83, -69), S(-43,-54), S(-21,-17), S(-10,  9) },
     { S( -71, -50), S(-22,-39), S(  0, -7), S(  9, 28) },
     { S( -25, -41), S( 18,-25), S( 43,  6), S( 47, 38) },
     { S( -26, -46), S( 16,-25), S( 38,  3), S( 50, 40) },
     { S( -11, -54), S( 37,-38), S( 56, -7), S( 65, 27) },
     { S( -63, -65), S(-19,-50), S(  5,-24), S( 14, 13) },
     { S(-195,-109), S(-67,-89), S(-42,-50), S(-29,-13) }
    },
    { // Bishop
     { S(-44,-58), S(-13,-31), S(-25,-37), S(-34,-19) },
     { S(-20,-34), S( 20, -9), S( 12,-14), S(  1,  4) },
     { S( -9,-23), S( 27,  0), S( 21, -3), S( 11, 16) },
     { S(-11,-26), S( 28, -3), S( 21, -5), S( 10, 16) },
     { S(-11,-26), S( 27, -4), S( 16, -7), S(  9, 14) },
     { S(-17,-24), S( 16, -2), S( 12,  0), S(  2, 13) },
     { S(-23,-34), S( 17,-10), S(  6,-12), S( -2,  6) },
     { S(-35,-55), S(-11,-32), S(-19,-36), S(-29,-17) }
    },
    { // Rook
     { S(-25, 0), S(-16, 0), S(-16, 0), S(-9, 0) },
     { S(-21, 0), S( -8, 0), S( -3, 0), S( 0, 0) },
     { S(-21, 0), S( -9, 0), S( -4, 0), S( 2, 0) },
     { S(-22, 0), S( -6, 0), S( -1, 0), S( 2, 0) },
     { S(-22, 0), S( -7, 0), S(  0, 0), S( 1, 0) },
     { S(-21, 0), S( -7, 0), S(  0, 0), S( 2, 0) },
     { S(-12, 0), S(  4, 0), S(  8, 0), S(12, 0) },
     { S(-23, 0), S(-15, 0), S(-11, 0), S(-5, 0) }
    },
    { // Queen
     { S( 0,-71), S(-4,-56), S(-3,-42), S(-1,-29) },
     { S(-4,-56), S( 6,-30), S( 9,-21), S( 8, -5) },
     { S(-2,-39), S( 6,-17), S( 9, -8), S( 9,  5) },
     { S(-1,-29), S( 8, -5), S(10,  9), S( 7, 19) },
     { S(-3,-27), S( 9, -5), S( 8, 10), S( 7, 21) },
     { S(-2,-40), S( 6,-16), S( 8,-10), S(10,  3) },
     { S(-2,-55), S( 7,-30), S( 7,-21), S( 6, -6) },
     { S(-1,-74), S(-4,-55), S(-1,-43), S( 0,-30) }
    },
    { // King
     { S(267,  0), S(320, 48), S(270, 75), S(195, 84) },
     { S(264, 43), S(304, 92), S(238,143), S(180,132) },
     { S(200, 83), S(245,138), S(176,167), S(110,165) },
     { S(177,106), S(185,169), S(148,169), S(110,179) },
     { S(149,108), S(177,163), S(115,200), S( 66,203) },
     { S(118, 95), S(159,155), S( 84,176), S( 41,174) },
     { S( 87, 50), S(128, 99), S( 63,122), S( 20,139) },
     { S( 63,  9), S( 88, 55), S( 47, 80), S(  0, 90) }
    }
  }
#endif
};

constexpr Score PBonus[RANK_NB][FILE_NB] =
  { // Pawn (asymmetric distribution)
   { },
   { S(  3,-10), S(  3, -6), S( 10, 10), S( 19,  0), S( 16, 14), S( 19,  7), S(  7, -5), S( -5,-19) },
   { S( -9,-10), S(-15,-10), S( 11,-10), S( 15,  4), S( 32,  4), S( 22,  3), S(  5, -6), S(-22, -4) },
   { S( -4,  6), S(-23, -2), S(  6, -8), S( 20, -4), S( 40,-13), S( 17,-12), S(  4,-10), S( -8, -9) },
   { S( 13, 10), S(  0,  5), S(-13,  4), S(  1, -5), S( 11, -5), S( -2, -5), S(-13, 14), S(  5,  9) },
   { S(  5, 28), S(-12, 20), S( -7, 21), S( 22, 28), S( -8, 30), S( -5,  7), S(-15,  6), S( -8, 13) },
   { S( -7,  0), S(  7,-11), S( -3, 12), S(-13, 21), S(  5, 25), S(-16, 19), S( 10,  4), S( -8,  7) }
  };
#ifdef CRAZYHOUSE
constexpr Score inHandBonus[PIECE_TYPE_NB] = {
    S(0, 0), S(52, 13), S(66, 30), S(4, 4), S(13, 3), S(25, 9)
};
#endif

#undef S

#ifdef CRAZYHOUSE
Score psq[VARIANT_NB][PIECE_NB][SQUARE_NB+1];
#else
Score psq[VARIANT_NB][PIECE_NB][SQUARE_NB];
#endif


// PSQT::init() initializes piece-square tables: the white halves of the tables are
// copied from Bonus[] and PBonus[], adding the piece value, then the black halves of
// the tables are initialized by flipping and changing the sign of the white scores.
void init() {

for (Variant var = CHESS_VARIANT; var < VARIANT_NB; ++var)
  for (Piece pc : {W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING})
  {
      Score score = make_score(PieceValue[var][MG][pc], PieceValue[var][EG][pc]);

      for (Square s = SQ_A1; s <= SQ_H8; ++s)
      {
          File f = File(edge_distance(file_of(s)));
          psq[var][ pc][s] = score + ((var == CHESS_VARIANT && type_of(pc) == PAWN) ? PBonus[rank_of(s)][file_of(s)]
                                                                                    : Bonus[var][pc][rank_of(s)][f]);
#ifdef RACE
          if (var == RACE_VARIANT)
              psq[var][~pc][horizontal_flip(s)] = -psq[var][pc][s];
          else
#endif
          psq[var][~pc][flip_rank(s)] = -psq[var][pc][s];
      }
#ifdef CRAZYHOUSE
      if (var == CRAZYHOUSE_VARIANT)
      {
          psq[var][ pc][SQ_NONE] = score + inHandBonus[type_of(pc)];
          psq[var][~pc][SQ_NONE] = -psq[var][pc][SQ_NONE];
      }
#endif
  }
}

} // namespace PSQT
