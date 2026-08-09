# Windows Vagrant helpers for c-weather.
# Requires: vagrant, VirtualBox, and a GUI-capable host for `run`.

set shell := ["bash", "-euo", "pipefail", "-c"]

box := "gusztavvargadr/windows-11"
msys_bash := 'C:\msys64\usr\bin\bash.exe'

# Проверить, что VirtualBox provider готов (модуль vboxdrv).
doctor:
    #!/usr/bin/env bash
    set -euo pipefail
    if [[ -e /dev/vboxdrv ]] || [[ -e /dev/vboxdrvu ]]; then
      echo "VirtualBox OK: $(VBoxManage --version 2>/dev/null | head -1)"
      exit 0
    fi
    k="$(uname -r)"
    echo "VirtualBox provider broken: /dev/vboxdrv missing (kernel $k)."
    echo
    echo "Ubuntu virtualbox-dkms 7.0.16 does not build on Linux 7.0"
    echo "(modpost: missing MODULE_IMPORT_NS for kvm_* symbols)."
    echo
    echo "Option A — reboot into 6.17 (vbox modules already installed):"
    echo "  sudo grub-reboot '1>2'   # adjust menu index if needed"
    echo "  # or at GRUB: Advanced options → 6.17.0-35-generic"
    echo "  sudo reboot"
    echo "  just doctor && just up"
    echo
    echo "Option B — stay on 7.0: install Oracle VirtualBox 7.2:"
    echo "  wget -qO- https://www.virtualbox.org/download/oracle_vbox_2016.asc \\"
    echo "    | sudo gpg --dearmor -o /usr/share/keyrings/oracle-virtualbox-2016.gpg"
    echo "  echo 'deb [arch=amd64 signed-by=/usr/share/keyrings/oracle-virtualbox-2016.gpg] https://download.virtualbox.org/virtualbox/debian noble contrib' \\"
    echo "    | sudo tee /etc/apt/sources.list.d/virtualbox.list"
    echo "  sudo apt-get remove -y virtualbox virtualbox-dkms virtualbox-qt || true"
    echo "  sudo apt-get update && sudo apt-get install -y virtualbox-7.2"
    echo "  sudo modprobe vboxdrv"
    echo
    echo "Unstick dpkg if headers are half-configured:"
    echo "  sudo apt-get remove -y virtualbox-dkms || true"
    echo "  sudo dpkg --configure -a"
    exit 1

# Поднять Windows VM (первый раз скачает box и провижинит MSYS2).
up: doctor
    vagrant up --provider=virtualbox

# Собрать c-weather в Windows VM (MSYS2 MinGW64).
build: up
    vagrant winrm -s powershell -c "{{msys_bash}} -lc \"export PATH=/mingw64/bin:/usr/bin:\$PATH; cd /c/vagrant && make clean && make\""

# Запустить c-weather в Windows VM (нужен vb.gui / сессия на рабочем столе).
run: build
    vagrant winrm -s powershell -c "$env:Path = 'C:\\msys64\\mingw64\\bin;' + $env:Path; Start-Process -FilePath 'C:\\vagrant\\c-weather.exe' -WorkingDirectory 'C:\\vagrant'"

# Остановить VM без удаления дисков.
halt:
    vagrant halt

# SSH/WinRM shell в VM.
ssh:
    vagrant winrm -s powershell

# Освободить диск: destroy VM, удалить .vagrant/, Windows box и ~/.vagrant.d/tmp.
clean-disk:
    #!/usr/bin/env bash
    set -euo pipefail
    echo "Destroying VM..."
    vagrant destroy -f || true
    rm -rf .vagrant
    echo "Removing Windows box ({{box}})..."
    vagrant box remove "{{box}}" --all --force 2>/dev/null || true
    echo "Pruning stale global status / tmp..."
    vagrant global-status --prune >/dev/null || true
    rm -rf "${HOME}/.vagrant.d/tmp"
    echo
    echo "Done. Vagrant data left:"
    du -sh "${HOME}/.vagrant.d" 2>/dev/null || true
    vagrant box list || true
