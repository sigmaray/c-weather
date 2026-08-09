# -*- mode: ruby -*-
# vi: set ft=ruby :
#
# Windows guest for building/running c-weather (MSYS2 MinGW64, same stack as CI).

Vagrant.configure("2") do |config|
  # windows-10 VirtualBox artifacts return 404; windows-11 is published on CDN.
  config.vm.box = "gusztavvargadr/windows-11"
  config.vm.box_version = "2607.1.0"
  config.vm.guest = :windows
  config.vm.communicator = "winrm"

  config.winrm.username = "vagrant"
  config.winrm.password = "vagrant"
  config.winrm.timeout = 1800
  config.vm.boot_timeout = 1800

  # Project root -> C:\vagrant (/c/vagrant in MSYS2)
  config.vm.synced_folder ".", "/vagrant"

  config.vm.provider "virtualbox" do |vb|
    vb.name = "c-weather-windows"
    vb.gui = true
    vb.memory = 4096
    vb.cpus = 2
    vb.linked_clone = true
    vb.customize ["modifyvm", :id, "--vram", "128"]
    vb.customize ["modifyvm", :id, "--clipboard", "bidirectional"]
  end

  # Install MSYS2 via Chocolatey (available on gusztavvargadr boxes).
  config.vm.provision "shell", name: "msys2", privileged: true, inline: <<-POWERSHELL
    $ErrorActionPreference = "Stop"
    if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
      Set-ExecutionPolicy Bypass -Scope Process -Force
      [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
      Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    }
    if (-not (Test-Path "C:\\msys64\\usr\\bin\\bash.exe")) {
      choco install msys2 -y --no-progress
    }
  POWERSHELL

  # MinGW toolchain + GTK/curl (mirrors .github/workflows/ci.yml).
  config.vm.provision "shell", name: "mingw-deps", privileged: true, inline: <<-POWERSHELL
    $ErrorActionPreference = "Stop"
    $bash = "C:\\msys64\\usr\\bin\\bash.exe"
    if (-not (Test-Path $bash)) { throw "MSYS2 bash not found at $bash" }
    & $bash -lc "pacman -Sy --noconfirm"
    & $bash -lc "pacman -S --noconfirm --needed make mingw-w64-x86_64-gcc mingw-w64-x86_64-pkgconf mingw-w64-x86_64-gtk3 mingw-w64-x86_64-curl"
  POWERSHELL
end
