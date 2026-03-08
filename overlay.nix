final: prev: {
  simplifiedVideoLibraryRenamer = final.qt6Packages.callPackage ({qtbase,qt5compat,wrapQtAppsHook}: final.stdenv.mkDerivation {
    pname = "simplifiedVideoLibraryRenamer";
    version = "1.0";
    src = ./src;
    buildInputs = [ qtbase qt5compat ];
    nativeBuildInputs = [ wrapQtAppsHook ];
    configurePhase = ''
      qmake PREFIX=$out
    '';
  }) {};
}
