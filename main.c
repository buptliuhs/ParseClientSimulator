/*
 *  Copyright (c) 2015, Parse, LLC. All rights reserved.
 *
 *  You are hereby granted a non-exclusive, worldwide, royalty-free license to use,
 *  copy, modify, and distribute this software in source code or binary form for use
 *  in connection with the web services and APIs provided by Parse.
 *
 *  As with any software that integrates with the Parse platform, your use of
 *  this software is subject to the Parse Terms of Service
 *  [https://www.parse.com/about/terms]. This copyright notice shall be
 *  included in all copies or substantial portions of the software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <parse.h>
#include "json.h"

void sha256(const char *string, char* outputBuffer)
{
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, string, strlen(string));
  SHA256_Final(hash, &sha256);
  // hash
  FILE *fp;
  fp=fopen("./.hash", "w");
  fwrite(hash, sizeof(hash[0]), sizeof(hash)/sizeof(hash[0]), fp);
  fclose(fp);
  // base64
  fp = popen("openssl enc -base64 -in ./.hash", "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }
  /* Read the output a line at a time - output it. */
  char tmp[65];
  while (fgets(tmp, sizeof(tmp)-1, fp) != NULL) {
    strcat(outputBuffer, tmp);
  }
  pclose(fp);
  strsep(&outputBuffer, "\n");
}

static void print_depth_shift(int depth)
{
  int j;
  for (j=0; j < depth; j++) {
    printf("  ");
  }
}

static void process_value(json_value* value, int depth, int isLast);

static void process_object(json_value* value, int depth)
{
  int length, x;
  if (value == NULL) {
    return;
  }
  printf("{\n");
  length = value->u.object.length;
  for (x = 0; x < length; x++) {
    print_depth_shift(depth);
    printf("\"%s\": ", value->u.object.values[x].name);
    process_value(value->u.object.values[x].value, depth+1, (x==length-1) ? 1 : 0);
  }
  print_depth_shift(depth-1);
  printf("}");
}

static void process_array(json_value* value, int depth)
{
  int length, x;
  if (value == NULL) {
    return;
  }
  length = value->u.array.length;
  printf("[\n");
  for (x = 0; x < length; x++) {
    print_depth_shift(depth);
    process_value(value->u.array.values[x], depth, (x==length-1) ? 1 : 0);
  }
  print_depth_shift(depth-1);
  printf("]");
}

static void process_value(json_value* value, int depth, int isLast)
{
  if (value == NULL) {
    return;
  }
  switch (value->type) {
    case json_none:
      printf("none");
      break;
    case json_object:
      process_object(value, depth);
      break;
    case json_array:
      process_array(value, depth);
      break;
    case json_integer:
      printf("%10" PRId64, value->u.integer);
      break;
    case json_double:
      printf("%f", value->u.dbl);
      break;
    case json_string:
      printf("\"%s\"", value->u.string.ptr);
      break;
    case json_boolean:
      printf("%d", value->u.boolean);
      break;
    case json_null:
      printf("null");
      break;
  }
  if (isLast)
    printf("\n");
  else
    printf(",\n");
}

void printFormattedJsonString(const char *json) {
  json_char* jc = (json_char*)json;
  json_value* value = json_parse(jc, strlen(json));
  process_value(value, 1, 1);
}

void myRequestCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody) {
  printf("*************************************\n");
  printf("Installation updated\n");
  printFormattedJsonString(httpResponseBody);
  printf("*************************************\n");
}

void updateInstallation(ParseClient client, const char *loginId, const char *channels) {
  const char* installationId = parseGetInstallationId(client);
  char sha[65] = "";
  sha256(loginId, sha);
  char data[1024] = "";
  sprintf(data, "{\"installationId\": \"%s\", \"deviceType\": \"embedded\", \"deliveryId\": \"%s\", \"channels\": %s}", installationId, sha, channels);
  printf("*************************************\n");
  printf("Update installation\n");
  printFormattedJsonString(data);
  printf("*************************************\n");
  parseSendRequest(client, "POST", "/1/classes/_Installation", data, myRequestCallback);
}

static int count = 0;
void myPushCallback(ParseClient client, int error, const char *buffer) {
  if (error == 0 && buffer != NULL) {
    char message[1024];
    sprintf(message, "figlet -f small New Message - %d", count++);
    system(message);
    // printf("%s\n", buffer);
    printFormattedJsonString(buffer);
  }
}

int main(int argc, const char *argv[]) {
  if (argc != 5) {
    printf("Usage: %s loginId channels applicationId apiKey\n", argv[0]);
    printf("Sample: %s tony.liu '[\"channelA\",\"channelB\",\"channelC\"]' e9bvTXGQNrzh7dxbW78R2VDd9LSaZKUZ7XXXXXXX 8djNp1o4HC7Gy4L6Zc2FJKBBwNlyLL5mrXXXXXXX\n", argv[0]);
    return 1;
  }
  const char* loginId = argv[1];
  const char* CHANNELS = argv[2];
  const char* APPLICATION_ID = argv[3];
  const char* CLIENT_KEY = argv[4];

  ParseClient client = parseInitialize(APPLICATION_ID, CLIENT_KEY);
  char message[1024];
  sprintf(message, "figlet -c Welcome %s", loginId);
  system(message);

  parseSetPushCallback(client, myPushCallback);
  int result = parseStartPushService(client);
  if (result != 0)
    printf("Result: %d\n", result);

  updateInstallation(client, loginId, CHANNELS);
  parseRunPushLoop(client);

  return 0;
}

