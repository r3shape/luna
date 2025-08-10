![r3ngin](.external/.data/assets/textures/logo.png)

![GitHub Stars](https://img.shields.io/github/stars/r3shape/r3ngin?style=for-the-badge&label=stars&labelColor=black&color=white)
![License](https://img.shields.io/badge/mit-badge?style=for-the-badge&logo=mit&logoColor=white&label=License&labelColor=black&color=white)

---

### Relevant Links
| [Discord](https://discord.gg/kreGBCVsQQ) | [Reddit](https://www.reddit.com/r/r3shape/) | [Discussions](https://github.com/r3shape/r3ngin/discussions) | [FAQ](https://github.com/r3shape/r3ngin/wiki/r3ngin-FAQ) |


## Building the Engine Source

> **NOTE**: If you're using the `r3make` CLI tool, ensure **Python** is installed. For more information, check out the [r3make Repository](https://github.com/r3shape/r3make).

r3ngin includes a **pre-configured `r3make` file** for easy setup and compilation. **[r3make](https://github.com/r3shape/r3make)** is the recommended tool for fast, simple, efficient builds.

---

### Option 1: Build with **r3make** (Recommended)

1. **Install r3make**:  
   Install via **PyPI**:  
   ```bash
   pip install r3make
   ```

2. **Clone the Repository**:  
   ```bash
   git clone https://github.com/r3shape/r3ngin
   ```

3. **Navigate to the `r3ngin` directory:
   ```bash
   cd r3ngin
   ```

4. **Build the Engine**:  
   - Target: `r3ngin` builds the `r3n.core.dll` from `r3ngin/src/core`
   ```bash
   r3make -nf -v
   ```
   - Target: `Runtime` builds the `r3ngin.exe` from `r3ngin/src/r3n.runtime`
   ```bash
   r3make -nf -v -t runtime
   ```

5. **Output**:  
   The compiled `r3n.core.dll`, along with `r3ngin.exe` runtime will be located in the `build` directory.
---

### Option 2: Manual Compilation

For those who wish to **wield the source** themselves:

1. **Clone the Repository**:  
   ```bash
   git clone https://github.com/r3shape/r3ngin
   cd r3ngin
   ```

2. **Compile the Engine**:  
   Manually compile using **GCC**:  
   | Compile the core library
   ```bash
   gcc -c r3ngin/src/r3n.core/*.c -Iinclude -o build/*.o
   gcc -shared build/*.o -Lexternal/__data__/thirdparty -lgdi32 -lopengl32 -lr3kit -o build/r3n.core.dll
   ```
   | Compile the runtime
   ```bash
   gcc r3ngin/src/r3n.runtime/*.c -Iinclude -Lexternal/__data__/thirdparty -Lbuild -lr3n.core -lr3kit -o build/r3ngin.exe
   ```

<br>

## Building Projects with **r3ngin**

1. **Prepare Your Project**:  
   Set up your source files, including necessary paths.

2. **Compile with r3make or GCC**:  
   Example using **GCC**:  
   ```bash
   gcc my_project.c -r3ngin/r3ngin/include -Lr3ngin/build -L/r3ngin/external/__data__/thirdparty -lr3n.core -lr3kit -o my_project.exe
   ```

<br>

## r3ngin Contributors

<a href="https://github.com/r3shape/r3ngin/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=r3shape/r3ngin"/>
</a>

<br>

## License

r3ngin is licensed under the **MIT License**.
