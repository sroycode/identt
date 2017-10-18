# Sending mail and sms

## Summary

- Sending external mail/sms is handled outside the program
- An external program can call the endpoint _identt/identity/api/v1/getmailstosend to get what mails to send
- The extracted data format is typically the json-equivalent of MPayloadT in proto/Mail.proto
- Two example programs for extraction are provided , see src/tools/GetMail.cc and python/MailHandle.py
- Briefly , the extraction program sends a request with shared_secret

## Sorry

... for the sparse documentation, I will make a more detailed description when time permits.
