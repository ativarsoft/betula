/* Copyright (C) 2022 Mateus de Lima Oliveira */

#include "config.h"

#ifdef USE_VIRTUALIZATION

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#include <pollen/virt.h>

int vmDefine()
{
	virConnectPtr conn = NULL;
	virDomainPtr domain = NULL;
	char *xmlDesc = NULL;
	char *uri = NULL;
	int ret = 0;
	//createTemporaryFile(ctx, &executeVirshCreate);
	conn = virConnectOpenAuth(uri, virConnectAuthPtrDefault, 0);
	if (conn == NULL) {
		fprintf(stderr,
			"No connection to hypervisor: %s\n",
			virGetLastErrorMessage());
		return 1;
	}
	uri = virConnectGetURI(conn);
	if (!uri) {
		ret = 1;
		fprintf(stderr,
			"Failed to get URI for hypervisor connection: %s\n",
			virGetLastErrorMessage());
		goto disconnect;
	}
	printf("Connected to hypervisor at \"%s\"\n", uri);
	free(uri);
	domain = virDomainDefineXML(conn, xmlDesc);
disconnect:
	if (0 != virConnectClose(conn)) {
		fprintf(stderr,
			"Failed to disconnect from hypervisor: %s\n",
			virGetLastErrorMessage());
		ret = 1;
	}
	return ret;
}

int vmStart(const char *name)
{
	virConnectPtr conn = NULL;
	virDomainPtr domain = NULL;
	char *uri = NULL;
	int ret = 0;

	conn = virConnectOpenAuth(uri, virConnectAuthPtrDefault, 0);
	if (NULL == conn) {
		fprintf(stderr,
			"No connection to hypervisor: %s\n",
			virGetLastErrorMessage());
		return 1;
	}
	domain = virDomainLookupByName(conn, name);
	if (!domain) {
		fprintf(stderr,
			"Domain not found: %s\n",
			virGetLastErrorMessage());
		ret = 1;
		goto disconnect;
	}
	if (0 != virDomainCreate(domain)) {
		fprintf(stderr,
			"Failed to start domain: %s\n",
			virGetLastErrorMessage());
		ret = 1;
		goto disconnect;
	}
	virDomainFree(domain);
disconnect:
	if (0 != virConnectClose(conn)) {
		fprintf(stderr,
			"Failed to disconnect from hypervisor: %s\n",
			virGetLastErrorMessage());
		ret = 1;
	}
	return 0;
}

int vmDestroy(const char *name)
{
	virConnectPtr conn = NULL;
	virDomainPtr domain = NULL;
	char *uri = NULL;
	int ret = 0;

	conn = virConnectOpenAuth(uri, virConnectAuthPtrDefault, 0);
	if (NULL == conn) {
		fprintf(stderr,
			"No connection to hypervisor: %s\n",
			virGetLastErrorMessage());
		return 1;
	}
	domain = virDomainLookupByName(conn, name);
	if (!domain) {
		fprintf(stderr,
			"Domain not found: %s\n",
			virGetLastErrorMessage());
		ret = 1;
		goto disconnect;
	}
	if (0 != virDomainDestroy(domain)) {
		fprintf(stderr,
			"Failed to destroy domain: %s\n",
			virGetLastErrorMessage());
		ret = 1;
		goto disconnect;
	}
	virDomainFree(domain);
disconnect:
	if (0 != virConnectClose(conn)) {
		fprintf(stderr,
			"Failed to disconnect from hypervisor: %s\n",
			virGetLastErrorMessage());
		ret = 1;
	}
	return 0;
}

#endif /* USE_VIRTUALIZATION */
