{ pkgs ? import <nixpkgs> {} }:
let
in
  pkgs.mkShell {
    buildInputs = [
      pkgs.opcua-client-gui
      pkgs.node-red
      pkgs.mqttx
      pkgs.pre-commit
    ];
}
