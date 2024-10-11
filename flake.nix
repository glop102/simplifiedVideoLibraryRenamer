{
    #inputs.nixpkgs = {
    #    type = "indirect";
    #    id = "nixpkgs";
    #};
    outputs = {self,nixpkgs}:
    with import nixpkgs { system = "x86_64-linux"; }; {
      packages."x86_64-linux" = {
        simplifiedVideoLibraryRenamer = pkgs.qt6Packages.callPackage ({ stdenv, qtbase, qmake, qt5compat, wrapQtAppsHook }: 
          stdenv.mkDerivation {
            name = "simplifiedVideoLibraryRenamer";
            version = "1.0";

            buildInputs = [ qtbase qmake ];
            nativeBuildInputs = [ wrapQtAppsHook ];
            src = ./.;
        }) {};
      };
      #apps."x86_64-linux" ={
      #  default = {
      #    type = "app";
      #    program = self.packages."x86_64-linux".simplifiedVideoLibraryRenamer;
      #  };
      #};
      devShells."x86_64-linux".default = mkShell {
        buildInputs = [ qt6.full qtcreator ];
      };
    };
}
