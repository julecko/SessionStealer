# SessionStealer

SessionStealer is a powerful command-line tool designed to extract and import browser session data, specifically cookies, from popular web browsers. Whether you're a developer debugging web applications, a security researcher analyzing browser behavior, or someone who needs to migrate sessions between browsers, SessionStealer provides a clean and efficient way to handle browser cookies.

## Features

- **Multi-Browser Support**: Works with Microsoft Edge, Mozilla Firefox (Chrome in later releases).
- **Dual Operations**: Export cookies from browsers to CSV files or import cookies from CSV files into browsers.
- **Automatic Discovery**: Automatically detects installed browsers on your system.
- **Modular Architecture**: Built with DLLs for easy extension to new browsers.
- **Cross-Platform Build**: Uses CMake for building on Windows and potentially other platforms.
- **SQLite Integration**: Leverages SQLite for robust cookie database handling.

## How It Works

SessionStealer operates through a modular architecture that separates concerns for maintainability and extensibility:

### Core Components

1. **Main Executable (`src/`)**: The command-line interface that orchestrates the entire operation. It parses user arguments, discovers available browsers, and delegates tasks to appropriate browser-specific DLLs.

2. **Browser Discovery (`dlls/discovery/`)**: A dedicated module that scans your system for installed browsers. It checks registry entries and common installation paths to identify supported browsers and their data directories.

3. **Browser-Specific DLLs**:
   - **Edge DLL (`dlls/edge/`)**: Handles Microsoft Edge's cookie database, which is queried through remote debugging websocket.
   - **Firefox DLL (`dlls/firefox/`)**: Manages Firefox's cookie storage, also SQLite-based but with different schema.
   - **Chromium DLL (`dlls/chromium/`)**: Provides utilities for Chromium-based browsers (Chrome, Edge, etc.), including WebSocket connections for potential remote debugging integration.

4. **Shared Utilities (`shared/`)**: Common functions for cookie parsing, CSV handling, and data manipulation that all browser modules can use.

### Operation Flow

When you run `sessionstealer export`:

1. The discovery module scans for installed browsers.
2. You select a browser (or it auto-selects if it's specified through command line arguments).
3. The appropriate DLL loads and connects to the browser's cookie database either directly or through some workaround.
4. Cookies are extracted and written to a CSV file in a standardized format.

For `sessionstealer import`, the process reverses: cookies are read from CSV and inserted into the browser's database.

### Cookie Format

SessionStealer uses a comprehensive CSV format that captures all cookie attributes. When exporting, SessionStealer creates a CSV file with the following columns:
- `browser_type`: The browser the cookie came from
- `name`: Cookie name
- `value`: Cookie value
- `domain`: Domain the cookie applies to
- `path`: Path within the domain
- `expires`: Expiration timestamp
- `http_only`: Whether the cookie is HTTP-only
- `secure`: Whether the cookie requires HTTPS
- `session`: Whether it's a session cookie
- And more advanced attributes...

For importing, ensure your CSV follows this exact format. 
This format ensures compatibility across different browsers and preserves all security and behavioral attributes of cookies.

## Installation

### Prerequisites

- Windows operating system
- CMake (3.10 or later)
- Visual Studio with C/C++ support (for MSVC compiler)
- vcpkg package manager

### Building from Source

1. **Clone the repository** (if applicable) or ensure you have the source code.

2. **Build the project**:
   - On Windows, run the provided `build.bat` script:
     ```
     build.bat
     ```
     This will configure CMake and build the project in Release mode by default.

   - For custom builds, you can use the `build.bat` with options:
     ```
     build.bat -d    # Build in Debug mode
     build.bat -r    # Run the executable after building
     ```

### Post-Build Setup

After building, ensure the DLL files are in the same directory as the executable or in your system's PATH. The build process should handle this automatically.

### Basic Syntax

```
sessionstealer <command> [options]
```

### Commands

- `export`: Extract cookies from a browser to a CSV file
- `import`: Import cookies from a CSV file into a browser

### Options

- `-V, --version`: Display the program version
- `-h, --help`: Show help message
- `-v, --verbose`: Enable verbose output for debugging (not implemented yet)
- `-f, --file <filename>`: Specify the CSV file (default: `cookies.csv`)
- `-b, --browser <browser>`: Specify browser (edge, firefox, chrome). If not specified, auto-discovery will prompt for selection.

### Examples

**Export cookies from Edge with verbose output:**
```
sessionstealer export -v -b edge
```

**Import cookies into Firefox from a specific file:**
```
sessionstealer import -f my_cookies.csv -b firefox
```

**Export cookies with auto-discovery:**
```
sessionstealer export
```
(This will show available browsers and let you choose)

**Get help:**
```
sessionstealer --help
```

## Tips and Best Practices

1. **Backup First**: Always backup your browser's data before importing cookies, as the import process modifies the browser's cookie database.

2. **Browser Compatibility**: While SessionStealer supports multiple browsers, cookie behaviors can vary. Test thoroughly when migrating sessions.

3. **Security Considerations**: Be cautious with exported cookie files - they contain sensitive session data. Store them securely and delete when no longer needed.

4. **Verbose Mode**: Use `-v` flag during operations to get detailed output, which is helpful for troubleshooting issues.

5. **Database Locks**: Ensure the target browser is closed before importing cookies, as open browsers may lock their cookie databases.

6. **Large Cookie Stores**: For browsers with many cookies, the export/import process might take some time. Be patient and avoid interrupting the process.

7. **Version Compatibility**: Cookie database schemas can change between browser versions. If you encounter issues, check if you're using the latest version of SessionStealer.

8. **Testing**: Use the provided test suite (`tests/`) to verify functionality before using on important data.

## Architecture Details

For developers interested in extending SessionStealer:

- **DLL Interface**: Each browser DLL exports `export_browser` and `import_browser` functions with a standardized signature.
- **Discovery API**: The discovery DLL provides a `discover_browsers` function that returns a list of installed browsers.
- **Shared Library**: Common cookie handling code is in `shared/` to avoid duplication.
- **CMake Structure**: The project uses modern CMake with subdirectories for clean organization.

## Contributing

Contributions are welcome! Whether it's adding support for new browsers, improving the CSV format, or enhancing the CLI, feel free to submit pull requests.

## License

This project is licensed under a Non-Commercial Use License. Please refer to the LICENSE file for details.
