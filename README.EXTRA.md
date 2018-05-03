# Extra and modified functions

## EndPoint _identt/identity/api/v1/getmailstosend (new)

- Sending external mail/sms is handled outside the program
- An external program can call the endpoint _identt/identity/api/v1/getmailstosend to get what mails to send
- The extracted data format is typically the json-equivalent of MPayloadT in proto/Mail.proto
- Two example programs for extraction are provided , see src/tools/GetMail.cc and python/MailHandle.py
- Briefly , the extraction program sends a request with shared_secret in header

## Endpoint _identt/identity/api/v1/blobdata/getAccessKey (new)

- A client can get access key for storing arbitrary json data by getting an access key.
- Query is similar to _matrix/identity/api/v1/3pid/getValidated3pid ( sid, client-secret) for a validated session
- Note that if lookup_requires_key is set the accesskey will be needed for lookup and bulk lookup


## Endpoint _identt/identity/api/v1/blobdata/getBlobData (new)

- A client can store arbitrary json data by access key and tag using this
- Query requires accesskey, tag.
- Returns the jsondata and textdata  if ok

## Endpoint _identt/identity/api/v1/blobdata/setBlobData (new)

- A client can store arbitrary json data by access key and tag.
- Requires accesskey, tag , jsondata, textdata, returns status.

## EndPoint _matrix/identity/api/v1/lookup (changed)

- Query requires accesskey if lookup_requires_key is set in config 
- Server to server queries then need to set Shared-Secret in header ( not documented in help section)
- If lookup_uses_local is greater than zero in config localassoc is used for lookups

## EndPoint _matrix/identity/api/v1/bulk_lookup (changed)

- Query requires accesskey if lookup_requires_key is set in config 
- Server to server queries then need to set Shared-Secret in header ( not documented in help section)
- if bulk_parallel_lookups is greater than 0 then parallel lookups are done for data
- If lookup_uses_local is greater than zero in config localassoc is used for lookups

## EndPoint _matrix/identity/api/v1/store-invite (changed)

- Query requires accesskey if invite_requires_key is set in config 
- Server to server queries then need to set Shared-Secret in header ( not documented in help section)

## Sorry

... for the sparse documentation, I will make a more detailed description when time permits.
