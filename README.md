# Usage

If you are creating your own model feel free to fork this repository.
As you develop, please replace "model" with appropriately descriptive names for your variables, files, and functions.

# Installation

This model can be built by ROSS by sym-linking it into the ROSS/models directory and building with `-DROSS_BUILD_MODELS=ON`

``` shell
git clone https://github.com/ROSS-org/ROSS
git clone https://github.com/ROSS-org/template-model
cd ROSS/models
ln -s ../../template-model ./
cd ../
mkdir build
cmake ../ -DROSS_BUILD_MODELS=ON
make
```
