# Simulator of Parse.com

## Introduction
This simulator is for receiving notifications from Parse.com.

This simulator is built based on [Parse Embedded C SDKs](https://github.com/ParsePlatform/parse-embedded-sdks).

This simulator is written in C and can be run in Linux & OS X environment.

## Other Dependencies
* Figlet
* Environment variables: 
  * PARSE_APPLICATION_ID
  * PARSE_CLIENT_KEY

## Usage
$ run.sh loginId channels

### Sample
$ run.sh tony.liu '["CHANNEL_A","CHANNEL_B"]'

