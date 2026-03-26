{
  description = "Wallet interface for the Logos application";

  inputs = {
    logos-module-builder.url = "github:logos-co/logos-module-builder";
    logos-standalone-app.url = "github:logos-co/logos-standalone-app";
    nix-bundle-lgx.url = "github:logos-co/nix-bundle-lgx";
    wallet_module.url = "github:logos-co/logos-wallet-module";
  };

  outputs = inputs@{ logos-module-builder, logos-standalone-app, ... }:
    logos-module-builder.lib.mkLogosModule {
      src = ./.;
      configFile = ./metadata.json;
      flakeInputs = inputs;
      logosStandalone = logos-standalone-app;
    };
}
