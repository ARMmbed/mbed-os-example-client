// List of targets to compile
def targets = [
  "K64F"
  ]
  
// Map toolchains to compilers
def toolchains = [
  ARM: "armcc",
  GCC_ARM: "arm-none-eabi-gcc"
  ]

def confgurations = [
  "default",
  "thread"
  ]
  
def stepsForParallel = [:]

// Jenkins pipeline does not support map.each, we need to use oldschool for loop
for (int i = 0; i < targets.size(); i++) {
  for(int j = 0; j < toolchains.size(); j++) {
    for(int k = 0; k < configurations.size(); k++) {
      def target = targets.get(i)
      def toolchain = toolchains.keySet().asList().get(j)
      def compilerLabel = toolchains.get(toolchain)
      def config = configurations.get(k)
  
      def stepName = "${target} ${toolchain} ${config}"
      stepsForParallel[stepName] = buildStep(target, compilerLabel, toolchain, config)
    }
  }
}

timestamps {
  parallel stepsForParallel
}

def buildStep(target, compilerLabel, toolchain, configName) {
  return {
    stage ("${target}_${compilerLabel}_${configName}") {
      node ("${compilerLabel}") {
        deleteDir()
        dir("mbed-os-example-client") {
          checkout scm
          
          if ("${configName}" == "thread") {
            // Add IPV6 feature
            execute("sed -i 's/\"CLIENT\", \"COMMON_PAL\"/\"CLIENT\", \"IPV6\", \"COMMON_PAL\"/' mbed_app.json")
            // Change connection type to thread
            execute ("sed -i 's/"value": "ETHERNET"/\"value\": \"MESH_THREAD\"/' mbed_app.json")
            // Add atmel-rf-driver
            execute ("mbed add https://github.com/ARMmbed/atmel-rf-driver")
          }
          
          // Copy security.h to build
          mbed.getSecurityFile()
  
          execute ("mbed deploy --protocol ssh")
          execute ("mbed compile --build .build/${target}_${compilerLabel}_${configName}/ -m ${target} -t ${toolchain} -c")
          archive '**/mbed-os-example-client.bin'
        }
      }
    }
  }
}
