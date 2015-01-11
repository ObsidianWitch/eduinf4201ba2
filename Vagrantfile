VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|  
  config.vm.box = "ubuntu/trusty64"
  
  config.vm.provision :shell, path: "Vagrantprov.sh"

  config.vm.define "korra" do |korra|
    korra.vm.hostname = "korra"
    korra.vm.network "private_network", ip: "192.168.33.10"
  end
  
  config.vm.define "katara" do |katara|
    katara.vm.hostname = "katara"
    katara.vm.network "private_network", ip: "192.168.33.11"
  end
  
  config.vm.define "azula" do |azula|
    azula.vm.hostname = "azula"
    azula.vm.network "private_network", ip: "192.168.33.12"
  end
end
