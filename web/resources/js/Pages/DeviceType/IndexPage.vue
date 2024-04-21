<script setup>
import AppLayout from "@/Layouts/AppLayout.vue";
import DeviceTypeCard from "@/Components/DeviceTypeCard.vue";
import PrimaryButton from "@/Components/PrimaryButton.vue";
import DialogModal from "@/Components/DialogModal.vue";
import {ref} from "vue";
import TextInput from "@/Components/TextInput.vue";
import {router, useForm} from "@inertiajs/vue3";
import SecondaryButton from "@/Components/SecondaryButton.vue";

defineProps({
    'deviceTypes': {
        type: Array,
        default: () => ([])
    }
})

const isVisibleDialog = ref(false);
const closeDialog = () => {
    isVisibleDialog.value = false;
    form.name = ""
}

const formType = ref('create')
const form = useForm({
    id: "",
    name: ""
})

const handleSubmit = () => {
    let method = formType.value === 'create' ? 'post' : 'put'
    let dynamicRoute = formType.value === 'create' ? 'device-types.store' : 'device-types.update'
    form.submit(method, route(dynamicRoute, {device_type: form.id}), {
        onFinish: () => closeDialog(),
        only: ['deviceTypes']
    })
}

const handleEdit = (deviceType) => {
    formType.value = 'edit';
    form.id = deviceType.id;
    form.name = deviceType.name;

    isVisibleDialog.value = true
}

const handleDelete = (deviceType) => {
    console.log(deviceType)
    router.delete(route('device-types.destroy', {device_type: deviceType.id}), {
        only: ['deviceTypes']
    })
}

</script>

<template>
    <app-layout>

        <template #header>
            <div class="flex justify-between">
                <h2 class="font-semibold text-xl text-gray-800 dark:text-gray-200 leading-tight">
                    Device Types
                </h2>

                <primary-button @click="isVisibleDialog = true; formType = 'create'">Add new</primary-button>
            </div>
        </template>

        <div class="py-12">
            <div class="max-w-7xl mx-auto sm:px-6 lg:px-8">
                <div
                    class="grid sm:grid-cols-2 lg:grid-cols-3 gap-4 bg-white dark:bg-gray-800 overflow-hidden shadow-xl sm:rounded-lg p-6">
                    <template v-for="deviceType in deviceTypes">
                        <device-type-card :data="deviceType" @edit="handleEdit($event)" @delete="handleDelete($event)"/>
                    </template>
                </div>
            </div>
        </div>

        <dialog-modal :show="isVisibleDialog" @close="closeDialog">
            <template #title>
                Add new device type
            </template>

            <template #content>
                <text-input v-model="form.name" placeholder="Device type name"
                />
            </template>

            <template #footer>
                <SecondaryButton @click="closeDialog">
                    Cancel
                </SecondaryButton>

                <PrimaryButton
                    class="ms-3"
                    :class="{ 'opacity-25': form.processing }"
                    :disabled="form.processing"
                    @click="handleSubmit"
                >
                    Submit
                </PrimaryButton>
            </template>
        </dialog-modal>
    </app-layout>
</template>