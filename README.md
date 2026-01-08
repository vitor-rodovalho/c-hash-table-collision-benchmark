# Hash Collision Benchmark

![Language](https://img.shields.io/badge/Language-C-blue.svg)
![Area](https://img.shields.io/badge/Area-Data_Structures-red.svg)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A C-based mini project that implements and benchmarks three different **Open Addressing** collision resolution strategies for Hash Tables. The program runs simulations with varying load factors and dataset sizes to analyze the efficiency of each method regarding probe counts and collisions.



## 📋 Features

- **Hash Function**: Uses the Multiplication Method (`h(k) = floor(m * (k * A mod 1))`).
- **Collision Resolution Strategies**:
  1.  **Linear Probing**: Searches for the next available slot sequentially.
  2.  **Quadratic Probing**: Uses a polynomial function to skip slots to reduce primary clustering.
  3.  **Double Hashing**: Uses a second hash function to calculate the step size.
- **Statistical Analysis**: Calculates average probes per insertion, total collisions, and maximum collision depth.
- **Dynamic Sizing**: Automatically selects prime numbers for table sizes to optimize distribution.



## 🚀 Getting Started

### Prerequisites

- GCC Compiler.
- Standard C libraries (`stdlib`, `math`, etc.).


### Compilation

Because the project uses `<math.h>` functions (like `sqrt`), you must link the math library using the `-lm` flag.
```bash
gcc -std=c11 -O2 -Wall hash_collision_benchmark.c -o benchmark -lm
```


### Execution

Run the compiled executable:
```bash
./benchmark
```



## 📊 How It Works

The program performs the following steps:

1. **Setup**: Defines three test scenarios with different sample sizes ($N = 100$, $1000$, $10000$).
2. **Table Sizing**: Calculates an optimal table size based on a load factor of approximately  and finds the nearest prime number.
3. **Simulation**:
    * Generates a pool of random keys.
    * Inserts keys using all three methods simultaneously.
    * Repeats the process multiple times (default: 5 reps) to ensure statistical accuracy.
4. **Reporting**: Outputs a comparison table showing:
    * Average total probes.
    * Average probes per insertion (efficiency metric).
    * Total collisions.
    * Maximum probe sequence length.



## 📝 Example Output

```text
=== N = 1000 (reps = 5) ===
TABLE_SIZE chosen = 1429 (fator de carga aproximado = 0.70)
Metodo          | avg_total_sondagem | avg_sondagem/insercao | avg_colisao | avg_max_colisao
---------------------------------------------------------------------------------
Linear          |             1540 |          1.5400 |         340.00 |          8.00
Quadratico      |             1535 |          1.5350 |         338.00 |          6.00
DoubleHash      |             1510 |          1.5100 |         330.00 |          5.00

>>> RESULTADO: para N = 1000, o melhor metodo (menor avg_total_sondagem) foi: DoubleHash
```


---

## ⚠️ Language Note

Please note that the code comments are currently in **Portuguese**, as this project was originally developed for a university assignment.



## ✒️ Author

Developed by **Vitor Hugo Rodovalho**.


---

## 📜 License

MIT License.
