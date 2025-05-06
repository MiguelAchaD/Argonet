<img src="assets/Logo.svg" alt="Argonet Logo" width="200"/>

# Argonet
## 💿Installation
### 📦Pre-built
Use the [Releases](https://github.com/MiguelAchaD/Argonet/releases/latest) link to download a builded software version.
### 🔧From scratch
If you want to build it your own, first you need to clone the repository containing the proyect:
```bash
git clone https://github.com/MiguelAchaD/Argonet.git
cd Argonet
```

The dependencies can be installed manually or using the "install" proyect script.
### Manual
#### APT
```bash
sudo apt-get install make g++ openssl libcurl4-openssl-dev nlohmann-json3-dev libsqlite3-dev
```
### Automatic (this will only work with the supported package managers)
```bash
./installation/install.sh
```

To build the proyect in the proyect directory just use:
```bash
make
```
If you wish to install it globally then do:
```bash
make install
```
## Platform support
For now, it is supported for any Linux-based distributions.
## License
[MIT](LICENSE)
