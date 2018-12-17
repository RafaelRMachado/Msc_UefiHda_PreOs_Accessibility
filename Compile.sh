#!/bin/bash

pathToUdk=/home/rafael/Projetos/edk2

InstallPackages=false
CompileBaseTools=false
BuildMdePkg=true
GenerateSoundHeaderFiles=true
DeployPendrive=true

#####################################################################################################################

if $InstallPackages; then
    echo "Instalando Pre-requisitos"
    sudo apt-get install build-essential uuid-dev texinfo bison flex libgmp3-dev libmpfr-dev subversion nasm iasl qemu ipython ffmpeg
fi

if $CompileBaseTools; then
	echo "Compilando BaseTools"
	make -C $pathToUdk/BaseTools

	cd $pathToUdk
	export EDK_TOOLS_PATH=$pathToUdk/BaseTools
	. edksetup.sh BaseTools
fi

if $GenerateSoundHeaderFiles; then

    for f in AudioFiles/*.mp3
    do
        ffmpeg -i $f -acodec pcm_s16le -f s16le -ac 2 $f.raw
        python GenerateSoundFileHeaders.py $f.raw
        mv $f.raw.h BeneMusicPlayer/AudioHeaders/    
    done
fi

if $BuildMdePkg; then
	echo "Preparando MdePkg"
	cd $pathToUdk
	export EDK_TOOLS_PATH=$pathToUdk/BaseTools

	. edksetup.sh BaseTools
	build -p MdeModulePkg/MdeModulePkg.dsc -b RELEASE -t GCC48 -a X64 -m MdeModulePkg/Application/BeneMusicPlayer/BeneMusicPlayer.inf

	echo "Copiando arquivos para Qemu"
	cp $pathToUdk/Build/MdeModule/RELEASE_GCC48/X64/HdaConfig.efi $pathToUdk/run-ovmf/hda-contents/HdaConfig.efi
fi

if $DeployPendrive; then
	echo "Enviando para o Pendrive"
	sudo cp $pathToUdk/Build/MdeModule/RELEASE_GCC48/X64/BeneMusicPlayer.efi /media/rafael/Ubuntu\ 16.04.3\ LTS\ amd64/
	echo "Arquivo Enviado"
fi
