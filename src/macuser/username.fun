# USERNAME (shell function)

function username {
  : ${USERNAME:=${LOGNAME}}
  : ${USERNAME:=${USER}}
  : ${USERNAME:=${HOME##*/}}
  : ${USERNAME:=${MAIL##*/}}
  print -- ${USERNAME}
}


