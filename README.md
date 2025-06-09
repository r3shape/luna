![luna](external/__data__/__assets__/textures/logo.png)

![GitHub Stars](https://img.shields.io/github/stars/r3shape/luna?style=for-the-badge&label=stars&labelColor=black&color=white)
![License](https://img.shields.io/badge/mit-badge?style=for-the-badge&logo=mit&logoColor=white&label=License&labelColor=black&color=white)

---

### Relevant Links
| [Discord](https://discord.gg/kreGBCVsQQ) | [Reddit](https://www.reddit.com/r/r3shape/) | [Discussions](https://github.com/r3shape/luna/discussions) | [FAQ](https://github.com/r3shape/luna/wiki/luna-FAQ) |


## Building the Engine Source

> **NOTE**: If you're using the `r3make` CLI tool, ensure **Python** is installed. For more information, check out the [r3make Repository](https://github.com/r3shape/r3make).

luna includes a **pre-configured `r3make` file** for easy setup and compilation. **[r3make](https://github.com/r3shape/r3make)** is the recommended tool for fast, simple, efficient builds.

---

### Option 1: Build with **r3make** (Recommended)

1. **Install r3make**:  
   Install via **PyPI**:  
   ```bash
   pip install r3make
   ```

2. **Clone the Repository**:  
   ```bash
   git clone https://github.com/r3shape/luna
   ```

3. **Navigate to the `luna` directory:
   ```bash
   cd luna
   ```

4. **Build the Engine**:  
   - Target: `luna` builds the `lunacore.dll` from `src/luna/core`
   ```bash
   r3make -nf -v -t luna
   ```
   - Target: `Runtime` builds the `luna.exe` from `src/luna/runtime`
   ```bash
   r3make -nf -v -t Runtime
   ```

5. **Output**:  
   The compiled `lunacore.dll`, along with `luna.exe` runtime will be located in the `build` directory.
---

### Option 2: Manual Compilation

For those who wish to **wield the source** themselves:

1. **Clone the Repository**:  
   ```bash
   git clone https://github.com/r3shape/luna
   cd luna
   ```

2. **Compile the Engine**:  
   Manually compile using **GCC**:  
   | Compile the core library
   ```bash
   gcc -c src/luna/core/*.c -Iinclude -o build/*.o
   gcc -shared build/*.o -Lexternal/__data__/thirdparty -lgdi32 -lopengl32 -lSSDK -o build/lunacore.dll
   ```
   | Compile the runtime
   ```bash
   gcc src/luna/runtime/*.c -Iinclude -Lexternal/__data__/thirdparty -Lbuild -llunacore -lSSDK -o build/luna.exe
   ```

<br>

## Building Projects with **luna**

1. **Prepare Your Project**:  
   Set up your source files, including necessary paths.

2. **Compile with r3make or GCC**:  
   Example using **GCC**:  
   ```bash
   gcc my_project.c -luna/include/luna -Lluna/build -L/luna/external/__data__/thirdparty -llunacore -lSSDK -o my_project.exe
   ```

<br>

## luna Contributors

<a href="https://github.com/r3shape/luna/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=r3shape/luna"/>
</a>

<br>

## License

luna is licensed under the **MIT License**.
