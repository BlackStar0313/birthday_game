package com.anansimobile.nge.IAB.util_v2;

public class CatalogEntry {

	/**
	 * Each product in the catalog can be MANAGED, UNMANAGED, or SUBSCRIPTION.  MANAGED
	 * means that the product can be purchased only once per user (such as a new
	 * level in a game). The purchase is remembered by Android Market and
	 * can be restored if this application is uninstalled and then
	 * re-installed. UNMANAGED is used for products that can be used up and
	 * purchased multiple times (such as poker chips). It is up to the
	 * application to keep track of UNMANAGED products for the user.
	 * SUBSCRIPTION is just like MANAGED except that the user gets charged monthly
	 * or yearly.
	 */
	public enum Managed { MANAGED, UNMANAGED, SUBSCRIPTION }
	
	public String sku;
    public int nameId;
    public Managed managed;

    public CatalogEntry(String sku, int nameId, Managed managed) {
        this.sku = sku;
        this.nameId = nameId;
        this.managed = managed;
    }
}

