include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(IDENTT_STORE_SOURCES
	StoreBase.cc
	StoreLevel.cc
	StoreTrans.cc
	StoreCache.cc

	InviteTokenTable.cc
	EphemeralPublicKeyTable.cc
	PeerTable.cc
	PeerPubKeyTable.cc
	LocalAssocTable.cc
	GlobalAssocTable.cc
	ValidationSessionTable.cc
	TokenAuthTable.cc
	ParAvionTable.cc
	MailSentTable.cc
	AccessKeyTable.cc
	BlobStoreTable.cc

	InviteService.cc
	LookupService.cc
	MailSmsService.cc
	PubKeyService.cc
	ThreePidService.cc
	ValidateService.cc
	BlobDataService.cc
	AccessKeyService.cc
)

add_library(identt_store STATIC ${IDENTT_STORE_SOURCES})

target_link_libraries(identt_store
	identt_proto
	identt_crypto
)
