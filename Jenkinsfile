// List of targets to compile
def targets = [
  "K64F"
  ]
  
// Map toolchains to compilers
def toolchains = [
  ARM: "armcc",
  GCC_ARM: "arm-none-eabi-gcc"
  ]
  
def stepsForParallel = [:]

// Jenkins pipeline does not support map.each, we need to use oldschool for loop
for (int i = 0; i < targets.size(); i++) {
  for(int j = 0; j < toolchains.size(); j++) {
    def target = targets.get(i)
    def toolchain = toolchains.keySet().asList().get(j)
    def compilerLabel = toolchains.get(toolchain)

    def stepName = "${target} ${toolchain}"
    stepsForParallel[stepName] = buildStep(target, compilerLabel, toolchain)
  }
}

timestamps {
  parallel stepsForParallel
}

def buildStep(target, compilerLabel, toolchain) {
  return {
    node ("${compilerLabel}") {
      deleteDir()
      dir("mbed-os-example-client") {
        checkout scm
        
        // Add IPV6 feature
        execute("sed -i 's/\"CLIENT\", \"COMMON_PAL\"/\"CLIENT\", \"IPV6\", \"COMMON_PAL\"/' mbed_app.json")
        
        // Copy security.h to build
        mbed.getSecurityFile()
        
        // Add atmel-rf-driver
        execute ("mbed add https://github.com/ARMmbed/atmel-rf-driver")
        
        execute ("mbed deploy --protocol ssh")
        execute ("mbed compile -m ${target} -t ${toolchain} -c")
      }
    }
  }
}
