#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>

const int debug = 0;

union Float {
              float f;
              uint32_t u;
} f_u;

void print_bin32(int *bin) {
        if (!debug) return;
        for (int i = 0; i < 32; ++i)
        printf("%d ", bin[31 - i]);
        printf("\n");
}

void float_to_bin(float v, int *bin) {
        union Float f_u = {.f = v};
        for (int i = 0; i < 32; ++i) {
                bin[i] = (f_u.u >> i) & 1;
        }
}

void print_ieee754(float f) {
        int bit[32];
        float_to_bin(f, bit);

        printf("%f = | %d | ", f, bit[31]);
        for (int i = 0; i < 8; ++i)
                printf("%d ", bit[30-i]);
        printf("| ");
        for (int i = 0; i < 23; ++i)
                printf("%d ", bit[22-i]);
        printf("\n");
}

float bin_to_float(int *bin) {
        float v = 0.0;
        union Float f_u = {.f = v};
        for (int i = 0; i < 32; ++i) {
                f_u.u += (1 << i) * bin[i];
        }
        return f_u.f;
}

int get_exponent(const int *bit) {
        int exp = 0;
        for (int i = 0; i < 8; ++i) {
                exp += (1 << i) * bit[23+i];
        }
        return exp - 127;

}

void add(int *out, const int *b1, const int *b2) {
       
       int exp1 = get_exponent(b1);
       int exp2 = get_exponent(b2);


       // Mantissas of inputs including normalization bit
       int m1[32] = {0};
       int m2[32] = {0};

       // summed mantissa including normalization bit
       int mtmp[32] = {0};

       // Exponent of output
       int exp[32] = {0};

       for (int i = 0; i < 23; ++i) {
                m1[i] = b1[i];
                m2[i] = b2[i];
       }
       m1[23] = 1;
       m2[23] = 1;

       int rshift = exp1 > exp2 ? exp1 - exp2 : exp2 - exp1;

       // mantissa of smaller float
       int *md = exp1 > exp2 ? m2 : m1;

       // mantissa of bigger float
       int *mu = exp1 > exp2 ? m1 : m2;

       print_bin32(m1);
       print_bin32(m2);

       // Right shift the mantissa of the smaller number
       for (int i = 0; i < 32 - rshift; ++i) md[i] = md[i + rshift];

       print_bin32(m1);
       print_bin32(m2);

       // Add mantissas
       int val = 0;
       int r = 0;
       for (int i = 0; i < 32; ++i) {
                val = m1[i] + m2[i] + r;
                mtmp[i] = val & 1;
                r = val >> 1;

       }

       print_bin32(out);
       

       // Add exponent
       const int *bd = exp1 > exp2 ? b1 : b2;
       int r0 = mtmp[24]; 
       r = r0;
       for (int i = 23; i < 32; ++i) {
                val = bd[i] + r;
                exp[i] = val & 1;
                r = val >> 1;
       }

       print_bin32(exp);

       // Copy exponent bits to output
       for (int i = 23; i < 32; ++i) {
                out[i] = exp[i];
       }

       // Normalize result by right shifting the mantissa
       for (int i = 0; i < 23; ++i) mtmp[i] = mtmp[i + r0];

       // Copy mantissa bits to output (do not copy implicit bit)
       for (int i = 0; i < 23; ++i) {
                out[i] = mtmp[i];
       }
}

int main(int argc, char **argv) {
        {
                int bin1[32];
                int bin2[32];
                int out[32];
                float f1 = 0.5;
                float f2 = 1.5;
                float f3 = 0.0;

                float_to_bin(f1, bin1);
                float_to_bin(f2, bin2);

                add(out, bin1, bin2);

                print_ieee754(f1);
                print_ieee754(f2);

                f3 = bin_to_float(out);
                print_ieee754(f3);
                int bin3[32];
                float_to_bin(f1 + f2, bin3);
                print_ieee754(f1 + f2);
        }

        {
                int bin1[32];
                int bin2[32];
                int out[32];
                double tol = pow(2,-23);
                int fail = 0;
                int pass = 0;

                for (int i = 0; i < 10000; ++i) {
                        float f1 = rand() * 1.f / RAND_MAX;
                        float f2 = rand() * 1.f / RAND_MAX;
                        float f3 = 0.0;
                        float_to_bin(f1, bin1);
                        float_to_bin(f2, bin2);
                        add(out, bin1, bin2);
                        f3 = bin_to_float(out);

                        double d3 = (double)f1 + (double)f2;
                        if (fabs(f3 - d3) > tol) {
                                printf("f1 + f2 - (double)(f1 + f2) = %.12f \n", f1 + f2 - d3);
                                printf("f3      - (double)(f1 + f2) = %.12f \n", f3 - d3);
                                printf("f1 = %12.12f, f2 = %12.12f \n", f1, f2);
                                printf("f1 + f2 = %12.12f, f3 = %12.12f \n", f1 + f2, d3);
                                printf("f1 = ");
                                print_ieee754(f1);
                                printf("f2 = ");
                                print_ieee754(f2);
                                printf("f1 + f2 = ");
                                print_ieee754(f1 + f2);
                                printf("     f3 = ");
                                print_ieee754(f3);
                                printf("-------------------------------------------------------\n");
                                fail++;
                        } else pass++;
                }
                printf("Tests passed: %d, Tests failed: %d \n", pass, fail);
        }

}
