{
pkgs ? import <nixpkgs> { },
}:
pkgs.mkShell {
    buildInputs = with pkgs; [
        python313
        python313Packages.pillow
        gcc
        valgrind
    ];

    shellHook = ''
        export PYTHONPATH=$PWD:$PYTHONPATH
        alias py="python";
    '';
}

