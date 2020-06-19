#!/bin/bash -l

# Make undefined variables errors, print each command
set -eux

# Clean up directory
rm -f jenkins-hpx*

# Start the actual build
source .jenkins/cscs/slurm-constraint-${configuration_name}.sh

set +e
echo "${ghprbActualCommit}"
echo "${ghprbActualCommitAuthor}"
echo "${ghprbActualCommitAuthorEmail}"
echo "${ghprbPullDescription}"
echo "${ghprbPullId}"
echo "${ghprbPullLink}"
echo "${ghprbPullTitle}"
echo "${ghprbSourceBranch}"
echo "${ghprbTargetBranch}"
echo "${ghprbCommentBody}"
echo "${sha1}"
sbatch \
    --job-name="jenkins-hpx-${configuration_name}" \
    --nodes="1" \
    --constraint="${configuration_slurm_constraint}" \
    --partition="cscsci" \
    --time="02:00:00" \
    --output="jenkins-hpx-${configuration_name}.out" \
    --error="jenkins-hpx-${configuration_name}.err" \
    --wait .jenkins/cscs/batch.sh
set -e

# Print slurm logs
echo "= stdout =================================================="
cat jenkins-hpx-${configuration_name}.out

echo "= stderr =================================================="
cat jenkins-hpx-${configuration_name}.err

# Get build status
if [[ $(cat jenkins-hpx-${configuration_name}-ctest-status.txt) ]]; then
    github_commit_status="success"
else
    github_commit_status="failure"
fi

# Extract just the organization and repo names "org/repo" from the full URL
github_commit_repo="$(echo $ghprbPullLink | sed -n 's/https:\/\/github.com\/\(.*\)\/pull\/[0-9]*/\1/p')"

# Get the CDash dashboard build id
cdash_build_id="$(cat jenkins-hpx-${configuration_name}-cdash-build-id.txt)"

# Extract actual token from GITHUB_TOKEN (in the form "username:token")
github_token=$(echo ${GITHUB_TOKEN} | cut -f2 -d':')

# Set GitHub status with CDash url
.jenkins/cscs/set_github_status.sh \
    "${github_token}" \
    "${github_commit_repo}" \
    "${GIT_COMMIT}" \
    "${github_commit_status}" \
    "${configuration_name}" \
    "${cdash_build_id}"

exit $(cat jenkins-hpx-${configuration_name}-ctest-status.txt)
