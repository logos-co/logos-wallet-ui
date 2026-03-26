# logos-wallet-ui

Wallet interface for the Logos application — a Qt UI plugin built with [logos-module-builder](https://github.com/logos-co/logos-module-builder).

## How to Build

### Using Nix (Recommended)

```bash
# Build the plugin (default)
nix build

# Build only the library
nix build '.#lib'

# Run in logos-standalone-app
nix run .
```

### Development Shell

```bash
nix develop
```

**Note:** In zsh, quote the target (e.g., `'.#lib'`).

## Output Structure

**Library build (`nix build`):**
```
result/
└── lib/
    └── wallet_ui_plugin.so    # Logos Wallet UI plugin (.dylib on macOS)
```

## Requirements

- [Nix](https://nixos.org/) with flakes enabled
- All other dependencies are managed by `logos-module-builder`

## Dependencies

- Qt6 (Core, Widgets, RemoteObjects, Quick, QuickWidgets)
- logos-cpp-sdk
- logos-wallet-module
- zstd, krb5, abseil-cpp
