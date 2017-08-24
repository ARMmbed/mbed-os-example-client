properties ([[$class: 'ParametersDefinitionProperty', parameterDefinitions: [
  [$class: 'StringParameterDefinition', name: 'mbed_os_revision', defaultValue: 'latest', description: 'Revision of mbed-os to build. To access mbed-os PR use format "pull/PR number/head"']
  ]]])

try {
  echo "Verifying build with mbed-os version ${mbed_os_revision}"
  env.MBED_OS_REVISION = "${mbed_os_revision}"
  if (mbed_os_revision.matches('pull/\\d+/head')) {
    echo "Revision is a Pull Request"
  }
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
          def config_file = "mbed_app.json"

          if ("${configName}" == "thd") {
            config_file = "./configs/mesh_thread.json"
            // Update Thread channel and PANID
            execute("sed -i 's/\"mbed-mesh-api.thread-config-channel\": 22/\"mbed-mesh-api.thread-config-channel\": 18/' ${config_file}")
            execute("sed -i 's/\"mbed-mesh-api.thread-config-panid\": \"0x0700\"/\"mbed-mesh-api.thread-config-panid\": \"0xBAAB\"/' ${config_file}")
   
          }

          if ("${configName}" == "6lp") {
            config_file = "./configs/mesh_6lowpan.json"

            // Change channel for HW tests
            execute ("sed -i 's/\"mbed-mesh-api.6lowpan-nd-channel\": 12/\"mbed-mesh-api.6lowpan-nd-channel\": 17/' ${config_file}")

            //Use PANID filter
            execute ("sed -i '/6lowpan-nd-channel\":/a \"mbed-mesh-api.6lowpan-nd-panid-filter\": \"0xABBA\",' ${config_file}")
          }

          if ("${connectiontype}" == "MCR20") {
            // Replace default rf shield
            execute ("sed -i 's/\"value\": \"ATMEL\"/\"value\": \"MCR20\"/' ${config_file}")
          }

          // Copy security.h to build
          mbed.getSecurityFile()

          // Set mbed-os to revision received as parameter
          execute ("mbed deploy --protocol ssh")
          dir("mbed-os") {
            if (env.MBED_OS_REVISION.matches('pull/\\d+/head')) {
                // Use mbed-os PR and switch to branch created
                execute("git fetch origin ${env.MBED_OS_REVISION}:_PR_")
                execute("git checkout _PR_")
              } else {
                execute ("git checkout ${env.MBED_OS_REVISION}")
            }
          }
          execute ("mbed compile --build out/${target}_${toolchain}_${configName}_${connectiontype}/ -m ${target} -t ${toolchain} -c --app-config ${config_file}")
        }
        archive '**/mbed-os-example-client.bin'
        step([$class: 'WsCleanup'])
      }
    }
  }
}
