# Configuration

## Section system

- accesslog : access log for daemon identt_getmail
- errorlog : error log for daemon identt_getmail
- httphost : URL to access identt server
- shared_secret : key to access _identt/ functions

## Section sms_server

This section assumes the SMS API has a certain format, it will not work for you unless you change GetMail.cc
GetMail.cc assumes the SMS api is country specific and accepts GET parameters mobile and message
like http://localhost:9051/sendsms_india?mobile=9999999999&message=something+something+here

```
IN = http://localhost:9051/sendsms_india
US = http://localhost:9051/sendsms_usa
```

## Section sms_template

This section has the templates for various sends , sms is 

```
authtoken = {token} is your One Time Password; Please enter the code to complete the verification process.
```
