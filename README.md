# Mini-Supermarché — Caisse Management (C)

This is a C program that simulates a mini-supermarket management system (products, clients, and checkout).
It also includes ready-to-use text files so you can run the program with sample data.

## Project files
- `main.c` → the full program
- `data/produits.txt` → sample products data
- `data/clients.txt` → sample clients data
- `docs/RAPPORT_Mini-Supermarche.pdf` → project report

## How the program loads data (.txt)
The program reads the sample files when it starts (or from a menu option), using `fopen()`.

If your code uses:
- `fopen("data/produits.txt", "r")` and `fopen("data/clients.txt", "r")`
  ✅ keep files inside the `data/` folder

If your code uses:
- `fopen("produits.txt", "r")` and `fopen("clients.txt", "r")`
  ✅ move the files to the repository root (same place as `main.c`)

## Run on Windows (easy way)

### Option A: Code::Blocks (recommended for beginners)
1. Install Code::Blocks (with MinGW).
2. Open Code::Blocks → Create a new project → Console application (C).
3. Add your `main.c` file (Project → Add files).
4. Make sure the `data/` folder is next to your compiled program (or in the project folder).
5. Click Build & Run.

### Option B: VS Code + MinGW (terminal)
1. Install MinGW-w64 (GCC).
2. Open the project folder in VS Code.
3. Open Terminal in VS Code.
4. Compile:
   ```bash
   gcc -o app main.c
