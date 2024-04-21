<?php

namespace Database\Seeders;

use Illuminate\Database\Console\Seeds\WithoutModelEvents;
use Illuminate\Database\Seeder;
use App\Models\User;

class AdminSeeder extends Seeder
{
    /**
     * Run the database seeds.
     */
    public function run(): void
    {
        // Admin
        $admin = new User([
            'name' => 'Admin',
            'email' => 'admin@tailadmin.dev',
            'password' => bcrypt('password')
        ]);
        $admin->save();
    }
}
