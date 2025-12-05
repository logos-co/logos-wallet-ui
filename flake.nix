{
  description = "Logos Wallet UI - A Qt UI plugin for Logos Wallet";

  inputs = {
    # Follow the same nixpkgs as logos-liblogos to ensure compatibility
    nixpkgs.follows = "logos-liblogos/nixpkgs";
    logos-cpp-sdk.url = "github:logos-co/logos-cpp-sdk";
    logos-liblogos.url = "github:logos-co/logos-liblogos";
    logos-wallet-module.url = "github:logos-co/logos-wallet-module/200b82182387601b3344459abbf763d2e7698c50";
    logos-accounts-module.url = "github:logos-co/logos-accounts-module/e6af00f8f666193bc95005b05f6587fbec8fde0d";
    logos-capability-module.url = "github:logos-co/logos-capability-module";
  };

  outputs = { self, nixpkgs, logos-cpp-sdk, logos-liblogos, logos-accounts-module, logos-wallet-module, logos-capability-module }:
    let
      systems = [ "aarch64-darwin" "x86_64-darwin" "aarch64-linux" "x86_64-linux" ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f {
        pkgs = import nixpkgs { inherit system; };
        logosSdk = logos-cpp-sdk.packages.${system}.default;
        logosLiblogos = logos-liblogos.packages.${system}.default;
        logosAccountsModule = logos-accounts-module.packages.${system}.default;
        logosWalletModule = logos-wallet-module.packages.${system}.default;
        logosCapabilityModule = logos-capability-module.packages.${system}.default;
      });
    in
    {
      packages = forAllSystems ({ pkgs, logosSdk, logosLiblogos, logosAccountsModule, logosWalletModule, logosCapabilityModule }: 
        let
          # Common configuration
          common = import ./nix/default.nix { 
            inherit pkgs logosSdk logosLiblogos; 
          };
          src = ./.;
          
          # Library package
          lib = import ./nix/lib.nix { 
            inherit pkgs common src logosAccountsModule logosWalletModule logosSdk; 
          };
          
          # App package
          app = import ./nix/app.nix { 
            inherit pkgs common src logosLiblogos logosSdk logosAccountsModule logosWalletModule logosCapabilityModule;
            logosWalletUI = lib;
          };
        in
        {
          # Individual outputs
          logos-wallet-ui-lib = lib;
          app = app;
          
          # Default package
          default = lib;
        }
      );

      devShells = forAllSystems ({ pkgs, logosSdk, logosLiblogos, logosAccountsModule, logosWalletModule, logosCapabilityModule }: {
        default = pkgs.mkShell {
          nativeBuildInputs = [
            pkgs.cmake
            pkgs.ninja
            pkgs.pkg-config
          ];
          buildInputs = [
            pkgs.qt6.qtbase
            pkgs.qt6.qtremoteobjects
            pkgs.zstd
            pkgs.krb5
            pkgs.abseil-cpp
          ];
          
          shellHook = ''
            export LOGOS_CPP_SDK_ROOT="${logosSdk}"
            export LOGOS_LIBLOGOS_ROOT="${logosLiblogos}"
            echo "Logos Wallet UI development environment"
            echo "LOGOS_CPP_SDK_ROOT: $LOGOS_CPP_SDK_ROOT"
            echo "LOGOS_LIBLOGOS_ROOT: $LOGOS_LIBLOGOS_ROOT"
          '';
        };
      });
    };
}
