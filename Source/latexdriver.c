#include <stdlib.h>
#include "../Headers/latexdriver.h"

int install_pdflatex_evince(){
  // Checking if tools are installed.
  int result_pdf = system("which pdflatex > /dev/null 2>&1");
  int result_evince =  system("which evince > /dev/null 2>&1");
  if (result_pdf != 0 || result_evince != 0) {
    // Installing pdflatex and evince if not installed (requires root).
    // User must prompt the password in order to install.
    system("gnome-terminal --wait -- bash -c 'echo \"Enter your password to install pdflatex:\" && "
           "sudo apt-get update && sudo apt-get install -y texlive-latex-base && sudo apt-get install -y evince; exec bash'");
    // Checking if the installation was successful
    result_pdf = system("which pdflatex > /dev/null 2>&1");
    result_evince = system("which evince > /dev/null 2>&1");
    if (result_pdf != 0 || result_evince != 0){
      // Unsuccessfully installed.
      return 1;
    }
  }
  // Tools already installed.
  return 0;
}

// sudo apt update
// sudo apt-get install texlive-latex-recommended texlive-latex-extra -y
// sudo apt install graphviz ? Necessary ?
// sudo tlmgr install dot2texi ? Necessary ?
// graphviz.sty en el mismo directory que el .tex
// crear aut.dot
// dot -Tpdf -o aut.pdf aut.dot
// pdflatex --shell-escape main.tex
// https://jvgomez.github.io/pages/manually-install-latex-packages-in-ubuntu.html

