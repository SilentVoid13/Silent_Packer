{
  description = "Silent_Packer";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    ...
  } @ inputs: let
  in
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {inherit system;};
        buildInputs = with pkgs; [
          libmhash
        ];
        nativeBuildInputs = with pkgs; [
          clang
          pkg-config
          cmake
          nasm
        ];
      in {
        devShells.default = pkgs.mkShell {
          inherit buildInputs;
          inherit nativeBuildInputs;
        };
      }
    );
}
