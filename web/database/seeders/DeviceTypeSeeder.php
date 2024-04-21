<?php

namespace Database\Seeders;

use App\Models\DeviceType;
use Illuminate\Database\Console\Seeds\WithoutModelEvents;
use Illuminate\Database\Seeder;

class DeviceTypeSeeder extends Seeder
{
    /**
     * Run the database seeds.
     */
    public function run(): void
    {
        // Pet Feeder
        $petFeeder = new DeviceType([
            'name' => "Pet Feeder"
        ]);
        $petFeeder->save();
    }
}
