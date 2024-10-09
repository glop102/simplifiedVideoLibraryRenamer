{
    inputs.nixpkgs = {
        type = "indirect";
        id = "nixpkgs";
    };
    outputs = {self,nixpkgs}:
    with import nixpkgs { system = "x86_64-linux"; }; {
        simplifiedVideoLibraryRenamer = pkgs.qt6Packages.callPackage ({ stdenv, qtbase, qmake, qt5compat, wrapQtAppsHook }: 

        stdenv.mkDerivation {
            name = "simplifiedVideoLibraryRenamer";
            version = "1.0";

            buildInputs = [ qtbase qmake qt5compat ];
            nativeBuildInputs = [ wrapQtAppsHook ];
            src = ./.;
        }) {};
    };
}