#!/bin/bash
echo "Star project requires WIFI AP"
echo "WIFI SSID AND password will be hard-coded into binary"
echo "SSID:"
read ssid
echo "Password:"
read pass
cat >ssid.h<<EOF
#ifndef _ssid_h
#define _ssid_h
const char *ssid = "$ssid";
const char *password = "$pass";
#endif
EOF

echo "ssid.h was created. avoid commiting it to public repositories!"

