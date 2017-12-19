# Sending mail and sms

## EndPoint _identt/identity/api/v1/getmailstosend


- Sending external mail/sms is handled outside the program
- An external program can call the endpoint _identt/identity/api/v1/getmailstosend to get what mails to send
- The extracted data format is typically the json-equivalent of MPayloadT in proto/Mail.proto
- Two example programs for extraction are provided , see src/tools/GetMail.cc and python/MailHandle.py
- Briefly , the extraction program sends a request with shared_secret in header

## Endpoint _identt/identity/api/v1/blobdata/getAccessKey

- A client can get access key for storing arbitrary json data by getting an access key.
- Query is similar to _matrix/identity/api/v1/3pid/getValidated3pid ( sid, client-secret) for a validated session


## Endpoint _identt/identity/api/v1/blobdata/getBlobData

- A client can store arbitrary json data by access key and tag using this
- Query requires accesskey, tag.
- Returns the jsondata and textdata  if ok

## Endpoint _identt/identity/api/v1/blobdata/setBlobData

- A client can store arbitrary json data by access key and tag.
- Requires accesskey, tag , jsondata, textdata, returns status.

## Sorry

... for the sparse documentation, I will make a more detailed description when time permits.
