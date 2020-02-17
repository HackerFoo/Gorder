# { stdenv, fetchurl }:
with import <nixpkgs> {};

let
  stdenv_gcc9 = overrideCC stdenv gcc9;
  stdenv_gcc8 = overrideCC stdenv gcc8;
  stdenv_gcc7 = overrideCC stdenv gcc7;
  stdenv_gcc6 = overrideCC stdenv gcc6;
in
stdenv.mkDerivation rec {
  name = "gorder-${version}";
  version = "git";
  src = fetchGit {
   url = "https://github.com/HackerFoo/Gorder.git";
  };

  #doCheck = true;

  buildInputs = [ capnproto glibc.static ];

  meta = with stdenv.lib; {
    description = "Generate a node ordering that preserves the graph data locality";
    homepage = https://github.com/HackerFoo/Gorder;
    license = licenses.mit;
    platforms = platforms.all;
  };
}
