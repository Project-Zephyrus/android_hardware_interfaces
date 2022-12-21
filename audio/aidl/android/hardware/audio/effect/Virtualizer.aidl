/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.hardware.audio.effect;

import android.hardware.audio.effect.VendorExtension;

/**
 * Virtualizer specific definitions. An audio virtualizer is a general name for an effect to
 * spatialize audio channels.
 *
 * All parameters defined in union Virtualizer must be gettable and settable. The capabilities
 * defined in Virtualizer.Capability can only acquired with IEffect.getDescriptor() and not
 * settable.
 */
@VintfStability
union Virtualizer {
    /**
     * Effect parameter tag to identify the parameters for getParameter().
     */
    @VintfStability
    union Id {
        int vendorExtensionTag;
        Virtualizer.Tag commonTag;
    }

    /**
     * Vendor Virtualizer implementation definition for additional parameters.
     */
    VendorExtension vendor;

    /**
     * Capability supported by Virtualizer implementation.
     */
    @VintfStability
    parcelable Capability {
        /**
         * Virtualizer capability extension, vendor can use this extension in case existing
         * capability definition not enough.
         */
        VendorExtension extension;
        /**
         * Indicates whether setting strength is supported. False value indicates only one strength
         * is supported and setParameter() method will always return EX_ILLEGAL_ARGUMENT.
         */
        boolean strengthSupported;
    }

    /**
     * Minimal possible per mille strength.
     */
    const int MIN_PER_MILLE_STRENGTH = 0;

    /**
     * Maximum possible per mille strength.
     */
    const int MAX_PER_MILLE_STRENGTH = 1000;

    /**
     * The per mille strength of the virtualizer effect.
     *
     * If the implementation does not support per mille accuracy for setting the strength, it is
     * allowed to round the given strength to the nearest supported value. In this case {@link
     * #IEffect.getParameter()} method should return the rounded value that was actually set.
     *
     * The valid range for strength is [0, 1000].
     */
    int strengthPm;
}
