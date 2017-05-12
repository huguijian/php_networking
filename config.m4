PHP_ARG_ENABLE(networking, whether to enable networking support,
Make sure that the comment is aligned:
[  --enable-networking           Enable networking support])
if test "$PHP_NETWORKING" != "no"; then
  PHP_NEW_EXTENSION(networking,networking.c networking_server.c, $ext_shared)
fi
