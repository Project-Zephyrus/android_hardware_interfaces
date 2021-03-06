/*
 * Copyright (C) 2020 The Android Open Source Project
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

package android.hardware.neuralnetworks;

import android.hardware.neuralnetworks.Memory;

/**
 * This interface represents a device memory buffer.
 */
@VintfStability
interface IBuffer {
    /**
     * Sets the content of this buffer from a shared memory region.
     *
     * @param src The source shared memory region.
     * @param dimensions Updated dimensional information. If the dimensions of the IBuffer object
     *                   are not fully specified, then the dimensions must be fully specified here.
     *                   If the dimensions of the IBuffer object are fully specified, then the
     *                   dimensions may be empty here. If dimensions.size() > 0, then all dimensions
     *                   must be specified here, and any dimension that was specified in the IBuffer
     *                   object must have the same value here.
     * @throws ServiceSpecificException with one of the following ErrorStatus values:
     *     - DEVICE_UNAVAILABLE if driver is offline or busy
     *     - GENERAL_FAILURE if there is an unspecified error
     *     - INVALID_ARGUMENT if provided memory is invalid, or if the dimensions is invalid
     */
    void copyFrom(in Memory src, in int[] dimensions);

    /**
     * Retrieves the content of this buffer to a shared memory region.
     *
     * The IBuffer object must have been initialized before the call to IBuffer::copyTo. For more
     * information on the state of the IBuffer object, refer to IDevice::allocate.
     *
     * @param dst The destination shared memory region.
     * @throws ServiceSpecificException with one of the following ErrorStatus values:
     *     - DEVICE_UNAVAILABLE if driver is offline or busy
     *     - GENERAL_FAILURE if the IBuffer object is uninitialized, or there is an unspecified
     *       error
     *     - INVALID_ARGUMENT if provided memory is invalid
     */
    void copyTo(in Memory dst);
}
