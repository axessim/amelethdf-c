
pipeline
{
    agent none

    triggers
    {
        pollSCM('H H(0-7) * * *')
    }

    environment
    {
        GIT_BRANCH="${env.BRANCH_NAME}"
    }

    stages
    {
        stage('Build')
	{
            parallel
	    {
                stage('Linux (Centos 6.5)')
		{
                    agent { label 'centos65-64b' }

                    steps
		    {
                        deleteDir()

                        checkout_git("axsdeploy", "${GIT_BRANCH}")
                        checkout_git("amelethdf-c", "${GIT_BRANCH}")

                        dir("amelethdf-c")
                        {
                            sh '''python ../axsdeploy/bootstrap.py --config-file ./axsdeploy.json build'''
			}

			deleteDir()
                    }
		    post
		    {
                        failure
			{
                            sendMailError("Build Linux (Centos 6.5) failed")
                        }
                    }
                }

               	stage('Windows (win7-64b)')
                {
                    agent { label 'win7-64b' }

                    steps
                    {
                        deleteDir()

                        checkout_git("axsdeploy", "${GIT_BRANCH}")
                        checkout_git("amelethdf-c", "${GIT_BRANCH}")

                        dir("amelethdf-c")
                        {
                            bat '''python ..\\axsdeploy\\bootstrap.py --config-file .\\axsdeploy.json build'''
                        }

			deleteDir()
                    }
                    post
                    {
                        failure
                        {
                            sendMailError("Build Windows (win7-64b) failed")
                        }
                    }
                }
            }
        }

        stage("Repack et Deploy")
	{
            agent { label 'tyrion' }

            steps
	    {
                deleteDir()

                checkout_git("axsdeploy", "${GIT_BRANCH}")
                checkout_git("amelethdf-c", "${GIT_BRANCH}")

                dir("amelethdf-c")
		{
                    sh '''python ../axsdeploy/bootstrap.py --config-file ./axsdeploy.json build -r repack'''
                }
            }
        }
    }
}

def checkout_git(String name, String branch)
{
    dir(name)
    {
        checkout([$class: 'GitSCM',
            branches: [[name: "*/" + branch]],
            userRemoteConfigs: [[credentialsId: '5e9bd817-496c-4cc4-b135-1a76948645a2',
                                 url: "http://gitlab.ginkgosolutions.intra/axessim/" + name + ".git"]]])
    }
}

def sendMailError(String stage)
{
    mail subject: "[Fail] ${JOB_NAME} (${BUILD_NUMBER}) : " + stage,
        to: 'didier.roisse@ginkgosolutions.intra',
        cc: 'philippe.spinosa@ginkgosolutions.intra,nathanael.muot@ginkgosolutions.intra,thomas.strub@ginkgosolutions.intra,bruno.weber@ginkgosolutions.intra',
        body: "Please go to ${BUILD_URL}/console and verify the build."
}
