properties ([[$class: 'ParametersDefinitionProperty', parameterDefinitions: [
  [$class: 'StringParameterDefinition', name: 'mbed_os_revision', defaultValue: 'latest', description: 'Revision of mbed-os to build']
  ]]])

try {
  echo "Verifying build with mbed-os version ${mbed_os_revision}"
  env.MBED_OS_REVISION = "${mbed_os_revision}"
} catch (err) {
  def mbed_os_revision = "latest"
  echo "Verifying build with mbed-os version ${mbed_os_revision}"
  env.MBED_OS_REVISION = "${mbed_os_revision}"
}

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
  "def": ["ETH"],
  "thd": ["ATMEL", "MCR20"],
  "6lp": ["ATMEL", "MCR20"]
  ]

def connectiontypes = [
  "ETH",
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

          if ("${configName}" == "thd") {
            // Change device type to Thread router
            execute("sed -i 's/\"NANOSTACK\", \"LOWPAN_ROUTER\", \"COMMON_PAL\"/\"NANOSTACK\", \"THREAD_ROUTER\", \"COMMON_PAL\"/' mbed_app.json")
            // Change connection type to thread
            execute ("sed -i 's/\"value\": \"ETHERNET\"/\"value\": \"MESH_THREAD\"/' mbed_app.json")
            // Reuse 6lowpan channel to Thread channel
            execute("sed -i 's/\"mbed-mesh-api.6lowpan-nd-channel\": 12/\"mbed-mesh-api.thread-config-channel\": 18/' mbed_app.json")
            // Reuse 6lowpan channel page to Thread PANID
            execute("sed -i 's/\"mbed-mesh-api.6lowpan-nd-channel-page\": 0/\"mbed-mesh-api.thread-config-panid\": \"0xBAAB\"/' mbed_app.json")
          }

          if ("${configName}" == "6lp") {
            // Change connection type to 6LoWPAN
            execute ("sed -i 's/\"value\": \"ETHERNET\"/\"value\": \"MESH_LOWPAN_ND\"/' mbed_app.json")

            // Change channel for HW tests
            execute ("sed -i 's/\"mbed-mesh-api.6lowpan-nd-channel\": 12/\"mbed-mesh-api.6lowpan-nd-channel\": 17/' mbed_app.json")

            //Use PANID filter
            execute ("sed -i '/6lowpan-nd-channel\":/a \"mbed-mesh-api.6lowpan-nd-panid-filter\": \"0xABBA\",' mbed_app.json")
          }

          if ("${connectiontype}" == "MCR20") {
            // Replace default rf shield
            execute ("sed -i 's/\"value\": \"ATMEL\"/\"value\": \"MCR20\"/' mbed_app.json")
          }

          // Copy security.h to build
          mbed.getSecurityFile()

          // Set mbed-os to revision received as parameter
          execute ("mbed deploy --protocol ssh")
          dir("mbed-os") {
            execute ("git checkout ${env.MBED_OS_REVISION}")
          }
          execute ("mbed compile --build out/${target}_${toolchain}_${configName}_${connectiontype}/ -m ${target} -t ${toolchain} -c")
        }
        archive '**/mbed-os-example-client.bin'
        step([$class: 'WsCleanup'])
      }
    }
  }
}
