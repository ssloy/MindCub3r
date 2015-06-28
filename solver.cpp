#include <iostream>
#include <cstring>

unsigned char edges[12], corners[8]; // actual configuration of the cube

const unsigned char idx2char[] = "FRBLUD"; // we need to define an ordering to know how the cube rotates (stored in how_edges_move and how_corners_move arrays)
const unsigned char char2idx[] = {9,2,9,5,9,0,9,9,9,9,9,3,9,9,9,9,9,1,9,9,4,9,9,9,9,9};

// When we rotate one face, there will be 4 rotating edges with 2 incident facets and 4 rotating corners with 3 incident facets
// these arrays define the transformations of the cube indices for all 6 possible rotations
const unsigned char how_edges_move[6][8] = {4,8,16,15, 0,12,20,11, 5,9,17,12, 1,13,21,8, 6,10,18,13, 2,14,22,9, 7,11,19,14, 3,15,23,10, 0,3,2,1, 4,7,6,5, 20,21,22,23, 16,17,18,19};
const unsigned char how_corners_move[6][12] = {4,12,19,11, 0,16,23,7, 8,20,15,3, 8,5,13,16, 4,1,17,20, 0,9,21,12, 9,6,14,17, 5,2,18,21, 1,10,22,13, 7,15,18,10, 3,19,22,6, 11,23,14,2, 0,3,2,1, 8,11,10,9, 4,7,6,5, 20,21,22,23, 16,17,18,19, 12,13,14,15};

// hard-coded algorithms to assemble corresponding edges and corners
const char alg_DF_edge[]   [10] = {"F2 ", "U1 ", "U2 ", "U3 ", "U3 ", "R3 F1 R1 ", "U3 ", "L1 F3 L3 ", "D1 R3 D3 ", "D2 B3 D2 ", "D3 L3 D1 ", "F3 ", "F1 ", "D1 R1 D3 ", "D2 B1 D2 ", "D3 L1 D1 ", "F3 ", "R1 ", "B1 ", "L3 ", "", "R2 ", "B2 ", "D1 "};
const char alg_FR_edge[]   [34] = {"U2 ", "U3 ", "F3 U3 F1 U1 R1 U1 R3 ", "U1 ", "U1 ", "U2 ", "U3 ", "R1 U1 R3 U3 F3 U3 F1 ", "", "D1 B1 U2 B3 D3 R1 U3 R3 ", "D1 B2 D3 U1 R2 ", "D3 L3 U2 L1 D1 F3 U1 F1 ", "F3 U1 F1 U3 R1 U2 R3 U2 R1 U3 R3 ", "R2 U2 R2 U2 R2 ", "R1 F3 B1 U3 B2 U1 F1 B3 R3 B2 ", "F2 U2 F2 U2 F2 "};
const char alg_UF_edge[]   [40] = {"", "R2 U1 F1 B3 R2 B1 F3 U1 R2 ", "R2 U3 F1 B3 R2 B1 F3 U3 R2 ", "F2 U3 L1 R3 F2 R1 L3 U3 F2 ", "R3 F1 R1 F1 R2 B3 D3 F3 D1 F1 B1 R2 F2 ", "R3 F3 R2 D2 L2 B3 L2 D2 R3 ", "R1 U1 R3 U3 R3 L1 F1 R1 F3 L3 ", "L1 F1 L2 D2 R2 B1 R2 D2 L1 "};
const char alg_DRF_corner[][10] = {"U3 ", "F3 U2 F1 ", "F3 U3 F1 ", "R1 U2 R3 ", "F3 U3 F1 ", "U1 ", "R1 U2 R3 ", "R1 U3 R3 ", "R1 U1 R3 ", "F3 U1 F1 ", "F3 U2 F1 ", "U3 ", "F3 U2 F1 ", "R3 U3 R1 ", "B3 U3 B1 ", "L3 U3 L1 ", "R1 U2 R3 ", "B1 U1 B3 ", "L1 U1 L3 ", "F1 U1 F3 ", "", "B1 U1 B3 ", "L1 U2 L3 ", "L3 U3 L1 "};
const char alg_URF_corner[][40] = {"", "U3 F2 L2 F3 R3 F1 L2 F3 R1 F3 ", "B1 L3 B1 R2 B3 L1 B1 R2 B2 ", "U3 ", "F1 R2 F1 U3 L2 U1 F3 R2 F1 U3 L2 U1 F2 ", "L1 U1 L3 B2 D3 F1 R3 F3 D1 B2 U1 ", "U1 F1 U3 B3 U1 F3 U3 B1 ", "F1 R1 U1 R3 U3 F3 ", "B1 U2 B2 R1 D3 R1 D1 R2 B2 U2 B3 U2 ", "B1 U3 F1 U1 F3 B3 U2 F1 U1 F3 U2 ", "R1 B2 L3 D1 L3 D3 L2 B2 R3 U3 ", "L1 U1 F1 U3 F3 L3 U2 "};

// given a face index and a number of turns, rotate the face
void atomic_rotation(unsigned char face_idx, unsigned char count) {
    for (int i=0; i<12; i++) { // # edges
        for (int j=0; j<8; j++) { // 4*2 edge facets are to be moved
            if (how_edges_move[face_idx][j] == edges[i]) {
                edges[i] = how_edges_move[face_idx][(j+count)%4+(j/4)*4];
                break;
            }
        }
    }
    for (int i=0; i<8; i++) { // # corners
        for (int j=0; j<12; j++) { // 4*3 corner facets are to be moved
            if (how_corners_move[face_idx][j] == corners[i]) {
                corners[i] = how_corners_move[face_idx][(j+count)%4+(j/4)*4];
                break;
            }
        }
    }
}

// given a sequence string, split it into atomic rotations and print the moves
// the moves are to be corrected with the orientation of the cube
void apply_sequence(const char *sequence, unsigned char orientation) {
    while (*sequence) {
        unsigned char fi  = char2idx[*sequence-'A'];
        unsigned char cnt = *(++sequence) - '0';
        atomic_rotation(fi, cnt);

        if (fi<4) fi = (fi+orientation)%4; // U and D facets are not affected by the entire cube rotation
        std::cout << idx2char[fi] << *sequence << " ";
        sequence += 2;
    }
}

void rotate_entire_cube(void) {
    for (int i=0; i< 8; i++) corners[i] = (corners[i]+3)%4 + (corners[i]/4)*4;
    for (int i=0; i<12; i++)   edges[i] = (  edges[i]+3)%4 + (  edges[i]/4)*4;
}

int main(int argc, char *argv[]) {
    { // init edges and corners array from the command line
        if (21!=argc) {
            std::cerr << "Example of usage:" << std::endl << argv[0] << " DB RU BL RF LD FU BU LF RD BR FD UL RBU UBL BRD RUF DFL RFD DLB LFU" << std::endl;
            return -1;
        }

        for (int i=0; i<12; i++) {
            int edgenums[2][12] = {0,1,2,3,20,21,22,23,8,15,13,10,  4,5,6,7,16,17,18,19,12,11,9,14};
            char edgelist[] = {"UF UR UB UL FR RB BL LF DF DR DB DL FU RU BU LU RF BR LB FL FD RD BD LD "};
            char *ret = strstr(edgelist, argv[i+1]);
            int tmp = (ret-edgelist)/3;
            edges[tmp%12] = edgenums[tmp/12][i];
        }

        for (int i=0; i<8; i++) {
            int cornernums[3][8] = {0,1,2,3,20,23,22,21,  4,5,6,7,16,19,18,17,  8,9,10,11,12,15,14,13};
            char cornerslist[] = "UFR URB UBL ULF DRF DBR DLB DFL RUF BUR LUB FUL FDR RDB BDL LDF FRU RBU BLU LFU RFD BRD LBD FLD ";
            char *ret = strstr(cornerslist, argv[i+13]);
            int tmp = (ret-cornerslist)/4;
            corners[tmp%8] = cornernums[tmp/8][i];
        }
    }

    { // solve the cube
        for (int e=8; e<12; e++) { // D edges have numbers 8..11
            while (edges[e] != 20) { // solved FD edge has the position 20
                apply_sequence(alg_DF_edge[edges[e]], e-8);
            }
            rotate_entire_cube();
        }

        for (int c=4; c<8; c++) { // D corners have numbers 4..7
            while (corners[c] != 20) { // solved DRF corner has the position 20
                apply_sequence(alg_DRF_corner[corners[c]], c-4);
            }
            rotate_entire_cube();
        }

        for (int e=4; e<8; e++) { // middle edges have numbers 4..7
            while (edges[e] != 8) { // solved edge number 4 has the position 8
                apply_sequence(alg_FR_edge[edges[e]], e-4);
            }
            rotate_entire_cube();
        }

        for (int c=0; c<4; c++) { // U corners have numbers 0..3
            apply_sequence(alg_URF_corner[corners[c]], c);
            rotate_entire_cube();
        }

        for (int e=0; e<4; e++) { // U edges have numbers 0..3
            apply_sequence(alg_UF_edge[edges[e]], e);
            rotate_entire_cube();
        }
    }
    std::cout << std::endl;
}

