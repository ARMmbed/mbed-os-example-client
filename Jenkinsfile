// List of targets to compile
def morpheusTargets = [
  //"LPC1768",
  //"NUCLEO_F401RE",
  //"NRF51822",
  "K64F"
  ]
  
// Map morpheus toolchains to compiler labels on Jenkins
def toolchains = [
  ARM: "armcc",
  GCC_ARM: "arm-none-eabi-gcc",
  IAR: "iar_arm"
  ]
  
// Initial maps for parallel build steps
def stepsForParallel = [:]
 		 
// Jenkins pipeline does not support map.each, we need to use oldschool for loop
for (int i = 0; i < morpheusTargets.size(); i++) {
  for(int j = 0; j < toolchains.size(); j++) {
    def target = morpheusTargets.get(i)
    def toolchain = toolchains.keySet().asList().get(j)
    def compilerLabel = toolchains.get(toolchain)  
    
    def stepName = "${target} ${toolchain}"
    stepsForParallel[stepName] = morpheusBuildStep(target, compilerLabel, toolchain)
  }
}

/* Jenkins does not allow stages inside parallel execution, 
 * https://issues.jenkins-ci.org/browse/JENKINS-26107 will solve this by adding labeled blocks
 */
stage "parallel build"
try {
  // Actually run the steps in parallel - parallel takes a map as an argument, hence the above.
  parallel stepsForParallel
} catch (err) {
  currentBuild.result = 'FAILURE'
} 


//Create morpheus build steps for parallel execution
 def morpheusBuildStep(target, compilerLabel, toolchain) {
    return {
      node ("morpheus && ${compilerLabel}") {
        deleteDir()
        dir("mbed-client-quickstart-morpheus") {
          checkout scm
          if ("${compilerLabel}" == "iar_arm") {
            bat"mbed --help"
            bat "mbed add https://github.com/ARMmbed/mbed-os/#c4986eed9f42cbb3352c793d7a16d512d978d102 --protocol ssh"
            bat "rm -f security.h"
            writeFile file: 'security.h', text: '''#ifndef __SECURITY_H__
#define __SECURITY_H__
#include <inttypes.h>
#define MBED_DOMAIN ""
#define MBED_ENDPOINT_NAME "lwm2m-endpoint"
const uint8_t SERVER_CERT[] = "";
const uint8_t CERT[] = "";
const uint8_t KEY[] = "";
#endif //__SECURITY_H__'''

            bat "cat security.h"
            bat "mbed ls -a"
            // sh "mbed deploy --protocol ssh"
            bat "mbed compile --tests -m ${target} -t ${toolchain} -c"
            
          } else {
            sh "mbed --help"
            sh "mbed add https://github.com/ARMmbed/mbed-os/#c4986eed9f42cbb3352c793d7a16d512d978d102 --protocol ssh"
            sh "rm -f security.h"
            writeFile file: 'security.h', text: '''#ifndef __SECURITY_H__
#define __SECURITY_H__
#include <inttypes.h>
#define MBED_DOMAIN ""
#define MBED_ENDPOINT_NAME "lwm2m-endpoint"
const uint8_t SERVER_CERT[] = "";
const uint8_t CERT[] = "";
const uint8_t KEY[] = "";
#endif //__SECURITY_H__'''

            sh "cat security.h"
            sh "mbed ls -a"
            // sh "mbed deploy --protocol ssh"
            sh "mbed compile -m ${target} -t ${toolchain} -c"
          }
        }
        // Check for compiler warnings
        step([$class: 'WarningsPublisher', 
          consoleParsers: [[parserName: 'GNU C Compiler 4 (gcc)']], 
          canComputeNew: false, 
          canResolveRelativePaths: false, 
          defaultEncoding: '', 
          excludePattern: '', 
          includePattern: '', 
          messagesPattern: '', 
          healthy: '', 
          unHealthy: ''
        ])
      }
    }
 }
