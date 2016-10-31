// List of targets to compile
def targets = [
  "K64F",
  "NUCLEO_F429ZI",
  "UBLOX_EVK_ODIN_W2"
  ]
  
// Map toolchains to compilers
def toolchains = [
  ARM: "armcc",
  GCC_ARM: "arm-none-eabi-gcc",
  IAR: "iar_arm"
  ]

def configurations = [
  "default": ["ETHERNET"],
  "thread": ["ATMEL", "MCR20"],
  "6lowpan": ["ATMEL", "MCR20"]
  ]

def connectiontypes = [
  "ETHERNET",
  "ATMEL",
  "MCR20"
  ]

def stepsForParallel = [:]

// Jenkins pipeline does not support map.each, we need to use oldschool for loop
for (int i = 0; i < targets.size(); i++) {
  for(int j = 0; j < toolchains.size(); j++) {
    for(int k = 0; k < configurations.size(); k++) {
      for(int l = 0; l < connectiontypes.size(); l++) {

        def target = targets.get(i)
        def toolchain = toolchains.keySet().asList().get(j)
        def compilerLabel = toolchains.get(toolchain)
        def config = configurations.keySet().asList().get(k)
        def allowed_configs = configurations.get(config)
        def connectiontype = connectiontypes.get(l)

        def stepName = "${target} ${toolchain} ${config} ${connectiontype}"
        if(allowed_configs.contains(connectiontype)) {
          stepsForParallel[stepName] = buildStep(target, compilerLabel, toolchain, config, connectiontype)
        }
      }
    }
  }
}

timestamps {
  parallel stepsForParallel
}

def buildStep(target, compilerLabel, toolchain, configName, connectiontype) {
  return {
    stage ("${target}_${compilerLabel}_${configName}_${connectiontype}") {
      node ("${compilerLabel}") {
        deleteDir()
        dir("mbed-os-example-client") {
          checkout scm
          
          if ("${configName}" == "thread") {
            // Change device type to Thread router
            execute("sed -i 's/\"NANOSTACK\", \"LOWPAN_ROUTER\", \"COMMON_PAL\"/\"NANOSTACK\", \"THREAD_ROUTER\", \"COMMON_PAL\"/' mbed_app.json")
            // Change connection type to thread
            execute ("sed -i 's/\"value\": \"ETHERNET\"/\"value\": \"MESH_THREAD\"/' mbed_app.json")
          }

          if ("${configName}" == "6lowpan") {
            // Change connection type to 6LoWPAN
            execute ("sed -i 's/\"value\": \"ETHERNET\"/\"value\": \"MESH_LOWPAN_ND\"/' mbed_app.json")

            // Change channel for HW tests
            execute ("sed -i 's/\"mbed-mesh-api.6lowpan-nd-channel\": 12/\"mbed-mesh-api.6lowpan-nd-channel\": 18/' mbed_app.json")
          }

          if ("${connectiontype}" == "MCR20") {
            // Replace default rf shield
            execute ("sed -i 's/\"value\": \"ATMEL\"/\"value\": \"MCR20\"/' mbed_app.json")
          }

          // Copy security.h to build
          mbed.getSecurityFile()
  
          execute ("mbed deploy --protocol ssh")

          dir("mbed-os") {
            execute ("git fetch origin latest")
            execute ("git checkout FETCH_HEAD")
          }
          execute ("mbed compile --build out/${target}_${compilerLabel}_${configName}_${connectiontype}/ -m ${target} -t ${toolchain} -c")
        }
        archive '**/mbed-os-example-client.bin'
      }
    }
  }
}
